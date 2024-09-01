/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frmurcia <frmurcia@42barcelona.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/29 11:52:40 by frmurcia          #+#    #+#             */
/*   Updated: 2024/09/01 13:33:15 by frmurcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "server.hpp"
#include "client.hpp" 
#include "commands.hpp"

// Constructor de la clase Server
Server::Server(int port, const std::string& password) : port(port), password(password), client_counter(0) {
	// Hacemos una funcion socket. Su retorno es un fd. En la funcion, usamos ipv4, es un tipo de socket de tipo flujo (stream) y el protocolo es elegido por el sistema operativo, teniendo en cuenta el ipv y que es de flujo, sera un protocolo TCP
    socket_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server_fd == -1) {
				std::cerr << "Error: Failed to create socket. Return value: " << socket_server_fd << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "socket server = " << socket_server_fd << std::endl;

		// Creamos una estructura sockaddr_in, que recogera la informacion del socket del servidor
    struct	sockaddr_in server_info; //terminacion _in = internet
    std::memset(&server_info, 0, sizeof(server_info)); //Inicializamos todo con 0s
    server_info.sin_family = AF_INET; //prefijo sin_ : socket internet
		// la direccion ip. s_addr es el unico miembro de sin_addr
    server_info.sin_addr.s_addr = INADDR_ANY; 
		// htons: el numero que le enviamos pase al estandar de red: hexadecimal big-endian
    server_info.sin_port = htons(port);

		//socket_server_fd va asociado a un socket. Con el cast hacemos que server_info sea
		//tratado como un struct sockaddr. Con el bind, hacemos que socket_server_fd este 
		//asociado con la dirección y el puerto especificados en server_info
    if (bind(socket_server_fd, reinterpret_cast<struct sockaddr*>(&server_info), sizeof(server_info)) == -1) {
				std::cerr << "Error: Failed to bind socket. Return value: -1" << std::endl;
        close(socket_server_fd);
        exit(EXIT_FAILURE);
    }

		// Ponemos socket_server_fd en modo escucha. Maxima cola de 10
    if (listen(socket_server_fd, 10) == -1) {
				std::cerr << "Error: Failed to listen on socket. Return value: -1" << std::endl;
        close(socket_server_fd);
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port " << port << std::endl;

    // la funcion poll() supervisa múltiples descriptores de archivos (como sockets) 
		// y detecta eventos de interés.
		struct	pollfd server_pollfd;
    server_pollfd.fd = socket_server_fd; // Le decimos que suvervise socket_server_fd 
    server_pollfd.events = POLLIN; // Le decimos que sea sensible a POLLIN, es decir, que sea
																	 // sensible a si hay datos disponibles para leer en el fd.
    poll_fds.push_back(server_pollfd); // Añadir server_pollfd al conjunto de Descriptores
																			 //  Supervisados (poll_fds), que es un vector.
}

/**********
 * POLLIN se activa cuando algo está disponible para leer en el socket. Es el evento que estamos esperando.
 * revents es el campo que se rellena después de que poll() retorna, y nos dice qué eventos 
 * realmente ocurrieron en cada socket.
 * **********/
void Server::run() {
    while (true) {
        // Mostrar la cantidad de clientes conectados, excluyendo el socket del servidor
        size_t num_clients = poll_fds.size() - 1;  // Excluye el socket del servidor
        std::cout << "Currently monitoring " << num_clients << " clients." << std::endl;
        
        // Mostrar información sobre cada socket siendo monitoreado
        std::cout << "  poll_fds[0] (server socket).fd = " << poll_fds[0].fd
                  << ", revents = " << poll_fds[0].revents << std::endl;
        for (size_t i = 1; i < poll_fds.size(); ++i) {  // Empieza desde 1 para evitar el socket del servidor
            std::cout << "  poll_fds[" << i << "].fd = " << poll_fds[i].fd
                      << ", revents = " << poll_fds[i].revents << std::endl;
        }

        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);

        if (poll_count < 0) {
						std::cerr << "Error: Poll failed with return value: " << poll_count << std::endl;
            break;
        }

        std::cout << "Polling for events..." << std::endl;

        for (size_t i = 0; i < poll_fds.size(); ++i) {
            std::cout << "Checking poll_fds[" << i << "]: fd = " << poll_fds[i].fd
                      << ", revents = " << poll_fds[i].revents << std::endl;

            if (poll_fds[i].revents & POLLIN) {  // Hay datos disponibles para leer
                if (poll_fds[i].fd == socket_server_fd) {  // Actividad en el socket del servidor
                    std::cout << "New incoming connection detected on server socket." << std::endl;
                    acceptClient();  // Aceptar nueva conexión de cliente
                    std::cout << "Accepted new client connection." << std::endl;
                } else {  // Actividad en un socket de cliente
                    std::cout << "Data available from client with fd: " << poll_fds[i].fd << std::endl;
                    handleClient(poll_fds[i].fd);  // Manejar datos del cliente
                }
            }
        }
    }
}


void Server::acceptClient() {
    struct sockaddr_storage client_info;
    std::memset(&client_info, 0, sizeof(client_info));
    socklen_t client_info_len = sizeof(client_info);

    int client_fd = accept(socket_server_fd, reinterpret_cast<struct sockaddr*>(&client_info), &client_info_len);
    if (client_fd < 0) {
				std::cerr << "Error: Failed to accept new client. Return value: " << client_fd << std::endl;
        return;
    }

    std::cout << "Accepted new client, fd: " << client_fd << std::endl;
		
    struct pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN;
    poll_fds.push_back(client_pollfd);

    // Incrementar el contador de clientes y usarlo para crear un apodo único
    std::ostringstream oss;  // Crear flujo de salida para convertir el número a string
    oss << "Guest" << ++client_counter;  // Crear apodo único
    std::string default_nick = oss.str();  // Obtener el string resultante

		Client* new_client = new Client(client_fd);
    new_client->setNickname(default_nick);  // Asignar el apodo al cliente
		clients[client_fd] = new_client;

    std::cout << "Total clients being monitored: " << poll_fds.size() - 1 << std::endl;

    if (client_info.ss_family == AF_INET) {
        struct sockaddr_in* client_info_v4 = reinterpret_cast<struct sockaddr_in*>(&client_info);
        std::cout << "Client connected from ipv4 " << inet_ntoa(client_info_v4->sin_addr) << ":"
                  << ntohs(client_info_v4->sin_port) << std::endl;
    }
		else {
			std::cerr << "Unsupported address family." << std::endl;
		}
}


void Server::handleClient(int client_fd) {
    char	buffer[1024];
    int		bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);  // Reducir en 1 para dejar espacio para '\0'

    if (bytes_received <= 0) {
        std::cerr << "Client (fd " << client_fd << ") closed the connection." << std::endl;
        close(client_fd);
        removeClient(client_fd);
				return;
    }
		buffer[bytes_received] = '\0';  // Agregar el terminador nulo al final de los datos recibidos
		std::cout << "Received from client (fd " << client_fd << "): \"" << buffer << "\"" << std::endl;
		Client* client = clients[client_fd];  // Obtener el cliente correspondiente al fd
    client->appendToBuffer(buffer);  // Acumular la entrada en el buffer del cliente

    std::string command;
    // Procesar comandos completos (solo si hay '\r\n')
    while (client->hasCompleteCommand(command)) {
				std::cout << "Processing command: \"" << command << "\"" << std::endl;
        handleCommand(client_fd, command);  // Procesar el comando completo
    }

		if (!client->getInputBuffer().empty()) {
				std::cout << "Buffer incompleto esperando finalización con getInputBuffer().empty(): \"" << client->getInputBuffer() << "\"" << std::endl;
		}
    // Si todavía hay datos en el buffer pero sin '\r\n', espera más
		if (client->isFinishedLine()) {
				std::cout << "Buffer incompleto esperando finalización: " << client->getInputBuffer() << std::endl;
				client->clearFinishedLine();  // Limpiar el estado para la próxima entrada
    }


		/*
		buffer[bytes_received] = '\0';  // Agregar el terminador nulo al final de los datos recibidos
		// Divide el buffer en líneas de comando
    std::string input(buffer); // convertimos buffer a una string
    std::istringstream ss(input);
    std::string line;
    while (std::getline(ss, line)) {
        // Aquí procesas cada comando IRC
        handleCommand(client_fd, line);
    }*/   //Parte cambiada para manejar el bufffer.
}



void Server::removeClient(int client_fd) {
    // Eliminamos el cliente del mapa
    delete clients[client_fd];
    clients.erase(client_fd);

    // Eliminamos también del vector poll_fds
    for (std::vector<pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ) {
        if (it->fd == client_fd) {
            it = poll_fds.erase(it);  // Elimina el elemento y actualiza el iterador
        } else {
            ++it;  // Solo avanza el iterador si no eliminaste el elemento
        }
    }
}


void	Server::handleCommand(int client_fd, const std::string& command) {
    // Aquí analizamos el comando y llama a los métodos correspondientes de la clase Command
    std::istringstream ss(command);
    std::string cmd;
    ss >> cmd;

    for (size_t i = 0; i < cmd.size(); ++i) {
        cmd[i] = toupper(cmd[i]);
		}// Usamos toupper() de cctype


    if (cmd == "NICK") {
        std::string new_nick;
        ss >> new_nick; // Obtener el nuevo apodo del comando
				if (!new_nick.empty()) { // Verificar que el nuevo apodo no esté vacio
		        Command::handleNickCommand(*this, client_fd, new_nick);
				}
				else {
						std::string error_msg = "ERROR: NICK command requires a nickname.\r\n";
						send(client_fd, error_msg.c_str(), error_msg.size(), 0);
				}
    } else if (cmd == "JOIN") {
        Command::handleJoinCommand(*this, client_fd, ss); // Ajusta según la firma real
    } else if (cmd == "PRIVMSG") {
        Command::handlePrivmsgCommand(*this, client_fd, ss); // Ajusta según la firma real
    }
		else {
				std::string error_msg = "ERROR: Unknown or incomplete command.\r\n";
				send(client_fd, error_msg.c_str(), error_msg.size(), 0);
		}

    // Agregar más comandos según sea necesario
}


// Método para obtener el mapa de clientes
std::map<int, Client*>& Server::getClients() {
		return (clients);
}

// Método para obtener un cliente basado en su fd
Client* Server::getClient(int client_fd) {
		if (clients.find(client_fd) != clients.end()) {
				return (clients[client_fd]);
		}
		return (NULL);  // Devuelve NULL si no se encuentra
}

// Método para enviar un mensaje a todos los clientes excepto a uno
void	Server::broadcastMessage(const std::string& message, int exclude_fd) {
		std::map<int, Client*>::const_iterator it;
		for (it = clients.begin(); it != clients.end(); ++it) {
				if (it->first != exclude_fd) {  // Excluir el fd proporcionado
						send(it->first, message.c_str(), std::strlen(message.c_str()), 0);
				}
		}
}


