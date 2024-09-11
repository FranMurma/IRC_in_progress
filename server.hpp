#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>


class Client;

class Server {
private:
    int socket_fd;
    int port;
    std::string password;
    std::string serverName;
    std::string serverTime;
    std::vector<pollfd> poll_fds;
    std::map<int, Client*> clients;

    void setupSignalHandler();
    void pollSockets();
    void handlePollEvents();
    void acceptClient();
	void newClient(int client_fd);
    void handleClient(int client_fd);
    void handleWrite(int client_fd); // Añadido
    void cleanup();
    void setTime();

    // Funciones de manejo de comandos
    bool handlePassCommand(Client& client, const std::vector<std::string>& tokens);
    bool handleUserCommand(Client& client, const std::vector<std::string>& tokens);
    bool handlePingCommand(Client& client, const std::vector<std::string>& tokens);

public:
    Server(); // Constructor sin parámetros
    void start();
    void run();
	void configureServer(int port, std::string password);
	std::string getTime() const;
	void sendResponse(int client_fd, const std::string& message);
	std::string getServerName() const;
	std::string getPassword() const;
	void broadcastMessage(const std::string& message);



};

#endif // SERVER_HPP

