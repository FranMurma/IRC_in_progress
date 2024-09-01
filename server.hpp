#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>   // Para std::cerr, std::cout
#include <cstdlib>    // Para exit(), EXIT_FAILURE
#include <cerrno>     // Para errno
#include <sys/socket.h> // Para socket functions
#include <arpa/inet.h>  // Para sockaddr_in y funciones de conversión de direcciones
#include <string>     // Para std::string
#include <cstring>    // Para std::memset
#include <unistd.h>   // Para close()
#include <poll.h>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include "client.hpp"
#include "channel.hpp"

class Server {
private:
    int	port;                  // Número de puerto en el que el servidor escuchará
    std::string password;      // Contraseña para la conexión
    int	socket_server_fd;      // Descriptor de archivo del socket del servidor
		struct	sockaddr_in server_info; // Direccion del servidor
		struct	sockaddr_in client_info; //Direccion del cliente
		std::vector<struct pollfd> poll_fds;  // Vector para almacenar múltiples pollfd
		// Mapa para rastrear clientes, clave es el fd del cliente
		// Asocia el descriptor de archivo del cliente (client_fd, que es un int) 
		// con un puntero a un objeto Client.
		std::map<int, Client*> clients;
		int	client_counter;  // Contador para asignar apodos únicos

		std::map<std::string, Channel*> channels;

public:
    // Constructor
    Server(int port, const std::string& password);
		void	run();  // Declaración de la función run()
		void  serverLoop();  // Bucle principal que usa poll()
		// Método para aceptar conexiones de clientes
    void	acceptClient(); // Aceptar nuevas conexiones
		void	handleClient(int client_fd);  // Manejar la comunicación con los clientes
		void	removeClient(int client_fd);  // Eliminar clientes cuando se desconecten
		void	handleCommand(int client_fd, const std::string& command); // Discernir que comando tenemos
		std::map<int, Client*>& getClients();  // Obtener todos los clientes
    Client* getClient(int client_fd);  // Obtener un cliente por su descriptor de archivo
		// Enviar un mensaje a todos los clientes excep   to uno
    void	broadcastMessage(const std::string& message, int exclude_fd);

		void	sendMessage(int client_fd, const std::string& message);

		Channel* getChannel(const std::string& channel_name);
		void	addChannel(Channel* channel);
};

#endif // SERVER_HPP
