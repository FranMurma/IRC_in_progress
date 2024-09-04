// server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <poll.h>

class Server {
	private:
		void	configureServer(int port, std::string password); //Constructor del servidor
	    int		socket_fd;                 // Descriptor de archivo del socket del servidor
	    int port;                                   // Puerto en el que el servidor escucha
		std::string password;                              // Contraseña para autenticación
    std::map<int, Client*> clients;                     // Mapa de clientes completos
    std::map<int, ProvisionalClient*> provisional_clients; // Mapa de clientes provisionales
    std::vector<pollfd> poll_fds;                       // Vector para manejar los pollfd
    bool hexChatUserCreated;                            // Flag para verificar si el usuario de HexChat fue creado
    int client_counter;                                 // Contador para asignar apodos únicos a los clientes

    void setServer(int port, const std::string& password); // Configura el servidor
    void acceptClient();                 // Acepta un nuevo cliente
    void handleClient(int client_fd);    // Maneja la entrada del cliente
    void removeClient(int client_fd);    // Elimina un cliente del servidor
    void handleCommand(int client_fd, const std::string& command);  // Maneja los comandos recibidos
    void sendMessage(int client_fd, const std::string& message);    // Envía un mensaje a un cliente

    // Funciones para manejar comandos específicos
    void handlePassCommand(int client_fd, std::istringstream& ss);
    void handleNickCommand(int client_fd, std::istringstream& ss);
    void handleUserCommand(int client_fd, std::istringstream& ss);

    // Funciones auxiliares
    void sendWelcomeMessages(int client_fd, Client* client);        // Envía los mensajes de bienvenida
    Client* getClient(int client_fd);                               // Obtiene un cliente por su descriptor
    Client* findClientByNickname(const std::string& nickname);      // Busca un cliente por su nickname

public:
		void	configureServer(int port, std::string password); //Constructor del servidor
    ~Server();                                      // Destructor del servidor

    void start();   // Inicia el servidor
    void run();     // Corre el bucle principal del servidor
};

#endif // SERVER_HPP

