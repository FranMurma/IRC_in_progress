#include "utils.hpp"
#include "server.hpp"

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



	// Imprimir la dirección IP y el puerto del cliente que se ha conectado
	std::cout << "Client connected from "
		<< inet_ntoa(client_info.sin_addr) << ":"  // Convertir y mostrar la dirección IP del cliente
		<< ntohs(client_info.sin_port) << std::endl;  // Convertir y mostrar el puerto del cliente

    // Aquí manejarías la comunicación con el cliente usando client_fd
}

// Vamos a hacer algo similar con el cliente. La funcion accept es similar a bind en su estructura
void	Server::acceptClient() {
    struct sockaddr_in client_info; //Creamos una structura sockaddr_in, PERO:
				// Se deja inicialmente vacío porque su propósito es ser rellenado por 
				// el sistema operativo cuando un cliente se conecta con la funcion ***accept
    socklen_t client_info_len = sizeof(client_info); //un len para recibir puerto e IP

    int client_fd = accept(socket_server_fd, reinterpret_cast<struct sockaddr*>(&client_info), &client_info_len);
    if (client_fd < 0) {
        std::cerr << "Accept failed: " << std::strerror(errno) << std::endl;
        close(socket_server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected from "
              << inet_ntoa(client_info.sin_addr) << ":"
              << ntohs(client_info.sin_port) << std::endl;

    std::cout << "Raw client info - IP: " << client_info.sin_addr.s_addr
              << ", Port: " << client_info.sin_port << std::endl;

    // Aquí manejarías la comunicación con el cliente usando client_fd
}
