// commands.cpp
#include "commands.hpp"
#include "utils.hpp"
#include "responses.hpp"
#include "server.hpp"
#include "passCommand.hpp"
#include <iostream>
#include "nickCommand.hpp"
#include "userCommand.hpp"
#include "channel.hpp"
#include "joinCommand.hpp"
#include "pingCommand.hpp"
#include "pongCommand.hpp"

bool	processCommand(const std::string& command, Client& client, Server& server) {
	std::cout << "Cliente recibido: " << command << std::endl;
	std::vector<std::string> tokens = splitString(command, " ");
    if (tokens.empty())
		return (false);

    std::string commandName = tokens[0];

    if (commandName == "NICK") {
        std::cout << "Processing NICK command: " << command << std::endl;
        handleNickCommand(client, tokens, server);
    } else if (commandName == "PASS") {
        std::cout << "Processing PASS command: " << command << std::endl;
        handlePassCommand(client, tokens, server);
    } else if (commandName == "USER") {
        std::cout << "Processing USER command: " << command << std::endl;
        handleUserCommand(client, tokens, server);
    } else if (commandName == "JOIN") {
		std::cout << "Processing JOIN command: " << command << std::endl;
		handleJoinCommand(client, tokens, server);
	}
	else if (commandName == "PING") {
		handlePingCommand(client, tokens, server);
	}
	else if (commandName == "PONG") {
		handlePongCommand(client, tokens, server);
	}
	return (true);
}




