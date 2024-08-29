#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include "server.hpp"  // Incluir para el uso del servidor y sus funciones relacionadas

// Declara las funciones que manejan los comandos IRC

class Command {
public:
    static void handleNickCommand(Server& server, int client_fd, const std::string& new_nick);
    static void handleJoinCommand(Server& server, int client_fd, std::istringstream& ss);
    static void handlePrivmsgCommand(Server& server, int client_fd, std::istringstream& ss);
    // Otros comandos...
};


#endif // COMMANDS_HPP

