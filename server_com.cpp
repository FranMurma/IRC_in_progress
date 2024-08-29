

#include "utils.hpp"
#include "server.hpp"
#include "client.hpp" 

// Constructor de la clase Server
Server::Server(int port, const std::string& password) : port(port), password(password) {
	// Inicialmente, solo almacenamos los valores en los atributos de la clase
	socket_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	// el fd es una forma sencilla de manejar los sockets. AF_INETsignifica protocolo ipv4
	// SOCK_STREAM significa que es un socket de tiplo flujo, la informacion es secuencial, en orden
	// 0 nos indica que el protocolo va a ser el que por defecto cuadre con el socket (ipv4 secuencial)
	// o sea, sera un protocolo TPC: Protocolo orientado a la transmision
	if (socket_server_fd == -1) {
		std::cerr << "Socket server creation failed: " << std::strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
	}
	std::cout << "socket server = " << socket_server_fd << std::endl;

	struct sockaddr_in server_info; // Se define una estructura sockaddr_in que almacena la informacion del socket del servidor.
	std::memset(&server_info, 0, sizeof(server_info)); // Inicializamos la estructura con 0s
	server_info.sin_family = AF_INET; // Establece el tipo de dirección como IPv4
	server_info.sin_addr.s_addr = INADDR_ANY; // Le asignamos todas las IP y puertos.
	server_info.sin_port = htons(port); // Asignamos el puerto que hemos metido en la entrada argv[1]
																			// htons hace que el numero que le pasamos pase al estandar
																			// de red: hexadecimal big-endian 
																			// (los bytes más significativos primero
	// bind(fd_socket, puntero de una struct sockaddr(por eso esta casteado), sizeof(address))
  // bind() le dice al sistema operativo: "Este socket (socket_server_fd) debe asociarse con la dirección y el puerto especificados en server_info
	if (bind(socket_server_fd, reinterpret_cast<struct sockaddr*>(&server_info), sizeof(server_info)) < 0) { // casteamos porque server_info es una estructura sockaddr_in
		std::cerr << "Bind failed: " << std::strerror(errno) << std::endl;
		// Cerramos el socket para liberar recursos
		close(socket_server_fd);
		// Terminamos el programa indicando que ocurrió un error
		exit(EXIT_FAILURE);
	}
	// Ahora, el fd del socket esta vinculado con esa estructura, y lo ponemos a escuchar.
	// int listen(int sockfd, int backlog); el backlog es el numero de conexiones en cola que permite
	if (listen(socket_server_fd, 10) < 0) {
		std::cerr << "Listen failed: " << std::strerror(errno) << std::endl;
		close(socket_server_fd);
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listening on port " << port << std::endl;
	// acceptClient: Aceptar una conexión de cliente que esta en la cola del backlog, asignará 

	// a un nuevo socket para manejar la comunicación con ese cliente.
	acceptClient();
	std::cout << "Raw client_info.sin_addr.s_addr: " << client_info.sin_addr.s_addr << std::endl;
	std::cout << "Raw client_info.sin_port: " << ntohs(client_info.sin_port) << std::endl;




	// Imprimir la dirección IP y el puerto del cliente que se ha conectado
	std::cout << "Client connected from "
		<< inet_ntoa(client_info.sin_addr) << ":"  // Convertir y mostrar la dirección IP del cliente
		<< ntohs(client_info.sin_port) << std::endl;  // Convertir y mostrar el puerto del cliente

    // Aquí manejarías la comunicación con el cliente usando client_fd
}

// Vamos a hacer algo similar con el cliente. La funcion accept es similar a bind en su estructura
void	Server::acceptClient() {
//    struct sockaddr_in client_info; //Creamos una structura sockaddr_in, PERO:
				// Se deja inicialmente vacío porque su propósito es ser rellenado por 
				// el sistema operativo cuando un cliente se conecta con la funcion ***accept
		struct sockaddr_storage client_info;
		std::memset(&client_info, 0, sizeof(client_info));
		socklen_t client_info_len = sizeof(client_info); //un len para recibir puerto e IP

		std::cout << "client_info_len before accept: " << client_info_len << std::endl;


    int client_fd = accept(socket_server_fd, reinterpret_cast<struct sockaddr*>(&client_info), &client_info_len);

/*		std::cout << "client_info_len after accept: " << client_info_len << std::endl;

    if (client_fd < 0) {
        std::cerr << "Accept failed: " << std::strerror(errno) << std::endl;
        close(socket_server_fd);
        exit(EXIT_FAILURE);
    }
		else {
			std::cout << "client_fd: " << client_fd << std::endl;
		}*/
		if (client_info.ss_family == AF_INET) {
    // Es una dirección IPv4
    struct sockaddr_in* client_info_v4 = reinterpret_cast<struct sockaddr_in*>(&client_info);
    std::cout << "Client connected from "
              << inet_ntoa(client_info_v4->sin_addr) << ":"
              << ntohs(client_info_v4->sin_port) << std::endl;

    std::cout << "Raw client info - IP: " << client_info_v4->sin_addr.s_addr
              << ", Port: " << client_info_v4->sin_port << std::endl;
} else if (client_info.ss_family == AF_INET6) {
    // Es una dirección IPv6
    struct sockaddr_in6* client_info_v6 = reinterpret_cast<struct sockaddr_in6*>(&client_info);
    char addr_str[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &(client_info_v6->sin6_addr), addr_str, INET6_ADDRSTRLEN);
    std::cout << "Client connected from "
              << addr_str << ":"
              << ntohs(client_info_v6->sin6_port) << std::endl;

    std::cout << "Raw client info - IP: " << client_info_v6->sin6_addr.s6_addr
              << ", Port: " << client_info_v6->sin6_port << std::endl;
}


		// Creamos una struct pollfd, estandar c/cpp. pollfd tiene un fd, events y revents
		// Guarda datos en la estructura por campos, entre ellos un fd. 
		// 
		// Los campos son accesibles con: nombre_estructura.campo, como estructura.fd
    struct	pollfd client_pollfd;
		//Asociamos el client_fd con la estructura client_poll a traves del campo client_pollfd.fd
    client_pollfd.fd = client_fd;
		// POLLIN: quiere decir que seamos sensibles a si llegan datos para leer en el fd
		// client_fd, que nos llega a traves de client_pollfd.fd
    client_pollfd.events = POLLIN; // Especificamos que queremos monitorear si llegan 
							 // datos para leer. Si llegan, poll() marcará POLLIN en revents. 
    poll_fds.push_back(client_pollfd); // Añadimos el pollfd del cliente al vector poll_fds
		// Agregamos el cliente al mapa de clientes. El mapa son pares 
		// client_fd --- puntero a un objeto Client
    clients[client_fd] = new Client(client_fd);


    std::cout << "Client connected from "
              << inet_ntoa(client_info.sin_addr) << ":"
              << ntohs(client_info.sin_port) << std::endl;

    std::cout << "Raw client info - IP: " << client_info.sin_addr.s_addr
              << ", Port: " << client_info.sin_port << std::endl;

    // Aquí manejarías la comunicación con el cliente usando client_fd
}

void	Server::handleClient(int client_fd) {
    char	buffer[1024];
    int		bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received <= 0) {
        // El cliente ha cerrado la conexión o ha habido un error
        close(client_fd);
        removeClient(client_fd);
    } else {
        // Procesar los datos recibidos
        std::cout << "Received from client: " << buffer << std::endl;
    }
}

bool	shouldRemovePollfd(const pollfd& pfd, int client_fd) {
    return (pfd.fd == client_fd);
}

void Server::removeClient(int client_fd) {
    // Eliminamos el cliente del mapa
    delete clients[client_fd];
    clients.erase(client_fd);

    // Eliminamos también del vector poll_fds
    // Recorremos el vector poll_fds y eliminamos los elementos que coincidan con client_fd
		for (std::vector<pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ) {
			if (it->fd == client_fd) {
				it = poll_fds.erase(it);  // Elimina el elemento y actualiza el iterador
				}
			else {
				++it;  // Solo avanza el iterador si no eliminaste el elemento
				}
		}
}

/*
void	Server::removeClient(int client_fd) {
    // tenemos que borrar los datos de dos formas.Por una parte, 
		// borramos el objeto con delete clientes[]
		// Por otra borramos la informacion asociada al evento representado por client_fd.
    delete clients[client_fd];
    clients.erase(client_fd);
    // Tercera parte del borrado: eliminamos la supervisión de eventos para el client_fd, 
		// que ya no es válido.
    poll_fds.erase(std::remove_if(poll_fds.begin(), poll_fds.end(), [client_fd](const pollfd& pfd) {
        return (pfd.fd == client_fd);
    }), poll_fds.end());
}*/


/******
 * Campos Principales de pollfd
fd (File Descriptor):

Propósito: Este campo contiene el descriptor de archivo que quieres supervisar.
Descripción: El descriptor de archivo (fd) es un número entero que representa un recurso abierto, como un socket, un archivo, o una tubería. En el contexto de un servidor de sockets, fd será el descriptor de archivo que representa un socket específico, como el del cliente o el del servidor.
Uso en poll(): poll() revisará este fd para detectar si se han producido eventos en él, como datos disponibles para leer o la posibilidad de escribir sin bloquear.
events:

Propósito: Especifica los eventos que te interesan monitorear en el fd.
Descripción: events es un campo que configuras tú mismo antes de llamar a poll(). Indicas los tipos de eventos que deseas que poll() supervise en ese descriptor de archivo.
Valores Comunes:
POLLIN: Notifica cuando hay datos disponibles para leer.
POLLOUT: Notifica cuando puedes escribir datos sin bloquear (es decir, el socket está listo para enviar datos).
POLLERR: Notifica si ha ocurrido un error en el fd.
POLLHUP: Notifica si se ha colgado la conexión (por ejemplo, si el cliente cierra el socket).
Uso en poll(): poll() supervisa estos eventos y actualiza el campo revents si alguno de ellos ocurre.
revents (Returned Events):

Propósito: Indica qué eventos realmente ocurrieron en el fd después de que poll() retorna.
Descripción: Este campo es actualizado por poll() para indicar cuáles de los eventos que estabas supervisando realmente sucedieron.
Uso en poll(): Después de que poll() retorna, revisas este campo para determinar qué acciones tomar, como leer datos del socket si POLLIN está marcado.
Valores Comunes:
POLLIN: Significa que hay datos disponibles para leer.
POLLOUT: Significa que puedes escribir datos sin bloquear.
POLLERR: Indica que ocurrió un error en el fd.
POLLHUP: Indica que el otro extremo del socket ha cerrado la conexión.
Resumen
fd: El descriptor de archivo que estás supervisando.
events: Los eventos que quieres monitorear (por ejemplo, si hay datos disponibles para leer).
revents: Los eventos que realmente ocurrieron, actualizados por poll().
Ejemplo de Uso
cpp
Copy code
struct pollfd client_pollfd;
client_pollfd.fd = client_fd;          // Supervisar el socket del cliente
client_pollfd.events = POLLIN;         // Quiero ser notificado cuando haya datos para leer

// Después de llamar a poll()
if (client_pollfd.revents & POLLIN) {
    // Hay datos disponibles para leer en client_fd
}
Estos son los tres campos principales que son más importantes para entender y trabajar con poll(). Si estos tienen sentido, estás en un buen punto para implementar poll() en tu servidor.

*******/

/********
 * struct pollfd {
    int fd;         // Descriptor de archivo a ser monitoreado
    short events;   // Eventos de interés en el fd
    short revents;  // Eventos que realmente ocurrieron
};
Descripción de los Campos
fd: El descriptor de archivo que se va a supervisar. Este es el identificador de un recurso del sistema operativo, como un socket o un archivo.

events: Este campo indica qué eventos quieres que poll() supervise en el descriptor de archivo fd. Algunos valores comunes incluyen:

POLLIN: Indica que hay datos disponibles para leer.
POLLOUT: Indica que se puede escribir sin bloquear.
POLLERR: Indica que ha ocurrido un error.
POLLHUP: Indica que el descriptor de archivo ha sido "colgado" (hang up).
revents: Este campo es configurado por poll() después de su ejecución para indicar qué eventos realmente ocurrieron en el descriptor de archivo fd. Estos pueden incluir los mismos valores que en events, y poll() llenará este campo con los eventos que ocurrieron.

Uso en el Código
Cuando usas poll(), generalmente declaras un array o vector de pollfd para los descriptores de archivo que deseas supervisar. Luego, llamas a poll() con ese array y el número de elementos en él, y poll() modificará los campos revents para indicar qué descriptores están listos para las operaciones que especificaste en events.

Ejemplo Básico
Aquí tienes un ejemplo básico de cómo podrías usar pollfd:

cpp
Copy code
#include <poll.h>
#include <unistd.h>
#include <iostream>

int main() {
    struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO;  // Monitorizar la entrada estándar (teclado)
    fds[0].events = POLLIN;    // Esperar a que haya datos disponibles para leer

    int ret = poll(fds, 1, 5000);  // Esperar hasta 5 segundos para un evento

    if (ret > 0) {
        if (fds[0].revents & POLLIN) {
            std::cout << "Data is available to read" << std::endl;
        }
    } else if (ret == 0) {
        std::cout << "No data within 5 seconds." << std::endl;
    } else {
        std::cerr << "Poll error" << std::endl;
    }

    return 0;
}
********************/
