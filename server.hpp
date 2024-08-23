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

class Server {
private:
    int	port;                  // Número de puerto en el que el servidor escuchará
    std::string password;      // Contraseña para la conexión
    int	socket_server_fd;      // Descriptor de archivo del socket del servidor
		struct	sockaddr_in server_info; // Direccion del servidor
		struct	sockaddr_in client_info; //Direccion del cliente

public:
    // Constructor
    Server(int port, const std::string& password);
		// Método para aceptar conexiones de clientes
    void	acceptClient();

    // Métodos adicionales se agregarán aquí conforme sean necesarios
};

#endif // SERVER_HPP
