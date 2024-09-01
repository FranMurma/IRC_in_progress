#include "client.hpp"

// Constructor
Client::Client(int fd) : client_fd(fd), nickname("Guest") {
}

const std::string& Client::getInputBuffer() const {
		return (input_buffer);
}

void	Client::setInputBuffer(const std::string& buffer) {
		input_buffer = buffer;
}


// Método para obtener el descriptor de archivo del cliente
int	Client::getFd() const {
    return (client_fd);
}

const std::string Client::getNickname() const {
		return (nickname);
}

// Establecer un nuevo apodo
void	Client::setNickname(const std::string& new_nick) {
	nickname = new_nick;
}

void	Client::appendToBuffer(const std::string& data) {
    input_buffer += data;
		if (input_buffer.find("\r\n") != std::string::npos) {
			finished_line = true;
		}
}

bool	Client::hasCompleteCommand(std::string& command) {
		size_t pos = input_buffer.find("\r\n");
		if (pos == std::string::npos) {
				std::cout << "1: No \\r\\n encontrado, buscando solo \\n\n";
				pos = input_buffer.find('\n');  // También buscar solo '\n'
		}
		if (pos != std::string::npos) {
				std::cout << "2: Final de línea encontrado en la posición " << pos << "\n";
				// Extraer el comando y eliminarlo del buffer
				command = input_buffer.substr(0, pos);
				input_buffer.erase(0, pos + 1);  // Eliminar el comando y el fin de línea del buffer
				// Si el próximo carácter en el buffer es '\r', también lo eliminamos
				if (!input_buffer.empty() && input_buffer[0] == '\r') {
						input_buffer.erase(0, 1);
						std::cout << "3: Eliminado '\\r' extra en el buffer.\n";
				}
				std::cout << "Command extracted: \"" << command << "\" for client " << client_fd << std::endl;
				finished_line = false;
				return (true);

    }
		std::cout << "4: No se ha encontrado un comando completo aún en el buffer actual.\n";
    return (false);
}

bool	Client::isFinishedLine() const {
        return (finished_line);
}

void	Client::clearFinishedLine() {
        finished_line = false;
}

void Client::clearBuffer() {
		input_buffer.clear();
		finished_line = false; 
}

// Destructor
Client::~Client() {
		if (client_fd > 0) {
			// Cerrar el socket del cliente si es necesario
    	close(client_fd);
		}
}

/**************
 * cada objeto client sera un par formado por un fd, que es el accept del socket, y un puntero a un objeto tipo Client. Esto basicamente lo hacemos para tener un objeto por cada evento. 
 * Cada Objeto Client:

Sí: Cada objeto Client se asocia con un descriptor de archivo (fd) específico, que es el resultado de una llamada a accept() en el socket del servidor.
Puntero a Client: El fd actúa como la clave en el mapa clients, y el valor asociado es un puntero a un objeto Client.
¿Por Qué Hacemos Esto?:

Para Gestionar Múltiples Clientes: El objetivo principal es poder manejar múltiples clientes conectados al servidor simultáneamente.
Objeto por Cliente: Al crear un objeto Client para cada conexión aceptada, puedes encapsular toda la información y la lógica relacionada con ese cliente en particular. Esto podría incluir el estado del cliente, los datos que ha enviado, la información sobre el protocolo que estás implementando, etc.
Beneficios de Esta Estructura
Encapsulación: Cada cliente tiene su propio objeto Client, lo que facilita mantener la lógica y los datos relacionados con cada cliente separados y organizados.
Acceso Eficiente: Puedes acceder rápidamente al objeto Client de un cliente específico usando su fd como clave en el mapa clients.
Manejo de Eventos: Al tener un objeto Client para cada cliente, puedes manejar eventos específicos (como recibir datos, enviar mensajes, etc.) de manera ordenada y específica para cada cliente.
Ejemplo Visual
Imagina un servidor con tres clientes conectados:

cpp
Copy code
std::map<int, Client*> clients;
Cliente 1: client_fd = 5, Client* = puntero a un objeto Client
Cliente 2: client_fd = 7, Client* = puntero a otro objeto Client
Cliente 3: client_fd = 9, Client* = puntero a un tercer objeto Client
Acciones con los Objetos Client
Recepción de Datos:

Cuando poll() indica que hay datos disponibles en client_fd = 5, puedes manejar esos datos accediendo al objeto Client* correspondiente.
Envío de Datos:

Si necesitas enviar un mensaje a un cliente específico, puedes hacerlo a través de su objeto Client.
Desconexión:

Cuando un cliente se desconecta, eliminas el objeto Client asociado de la memoria y lo quitas del mapa clients.
Resumen Final
Cada cliente conectado es representado por un par en el mapa clients, donde la clave es el fd y el valor es un puntero a un objeto Client.
Este diseño te permite manejar eficientemente múltiples clientes, encapsulando toda la lógica relacionada con cada cliente en su propio objeto.

**********************************************************/
