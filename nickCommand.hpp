// nickCommand.hpp
#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include <string>
#include <vector>
#include "client.hpp"
#include "server.hpp"

// Declaración de la función para manejar el comando NICK
bool handleNickCommand(Client& client, const std::vector<std::string>& tokens, Server& server);

#endif // NICKCOMMAND_HPP

