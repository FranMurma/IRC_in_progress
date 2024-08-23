#include "utils.hpp"
#include "server.hpp"

// Constructor de la clase Server
Server::Server(int port, const std::string& password) : port(port), password(password) {
	  // Inicialmente, solo almacenamos los valores en los atributos de la clase
	socket_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_server_fd == -1) {
		std::cerr << "Socket server creation failed: " << std::strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
	}
	std::cout << "socket server = " << socket_server_fd << std::endl;

	struct sockaddr_in server_info; // Se define una estructura sockaddr_in que almacena la informacion del socket del servidor.
	std::memset(&server_info, 0, sizeof(server_info)); // Inicializamos la estructura con 0s
	server_info.sin_family = AF_INET; // Establece el tipo de dirección como IPv4
	server_info.sin_addr.s_addr = INADDR_ANY; // Escuchar en cualquier dirección
	server_info.sin_port = htons(port); // Convertir el puerto a formato de red
	
	// Vincular el socket del servidor a la dirección IP y puerto
  // bind() asocia el socket con la dirección y puerto especificados en server_info
	if (bind(socket_server_fd, reinterpret_cast<struct sockaddr*>(&server_info), sizeof(server_info)) < 0) { // casteamos porque server_info es una estructura sockaddr_in
		std::cerr << "Bind failed: " << std::strerror(errno) << std::endl;
		// Cerramos el socket para liberar recursos
		close(socket_server_fd);
		// Terminamos el programa indicando que ocurrió un error
		exit(EXIT_FAILURE);
	}
	// Poner el socket en modo escucha
	if (listen(socket_server_fd, 10) < 0) {
		std::cerr << "Listen failed: " << std::strerror(errno) << std::endl;
		close(socket_server_fd);
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listening on port " << port << std::endl;
	// Aceptar una conexión de cliente (esto es solo para pruebas)
	acceptClient();



	// Imprimir la dirección IP y el puerto del cliente que se ha conectado
	std::cout << "Client connected from "
		<< inet_ntoa(client_info.sin_addr) << ":"  // Convertir y mostrar la dirección IP del cliente
		<< ntohs(client_info.sin_port) << std::endl;  // Convertir y mostrar el puerto del cliente

    // Aquí manejarías la comunicación con el cliente usando client_fd
}

void Server::acceptClient() {
    struct sockaddr_in client_info;
    socklen_t client_info_len = sizeof(client_info);

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
