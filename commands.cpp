// commands.cpp
#include "commands.hpp"
#include "utils.hpp"
#include "responses.hpp"
#include "server.hpp"
#include "passCommand.hpp"
#include <iostream>
#include "nickCommand.hpp"
#include "userCommand.hpp"

bool processCommand(const std::string& command, Client& client, Server& server) {
	std::cout << "Cliente recibido: " << command << " (Longitud: " << command.length() << ")" << std::endl;
    std::vector<std::string> tokens = splitString(command, " ");
    if (tokens.empty()) return false;

    std::string commandName = tokens[0];

    if (commandName == "NICK") {
		std::cout << "Processing NICK command en commandName = NICK: " << command << std::endl;  // Verificar comando recibido
	//        return handleNickCommand(client, tokens, server);
		if (handleNickCommand(client, tokens, server)) {
            // Enviar respuestas de bienvenida si todos los comandos son correctos y el cliente se registra
            if (client.isRegistered()) {

                server.sendResponse(client.getSocketFD(), RPL_WELCOME(client.getNickname(), server.getServerName(), client.getHostname()));
                server.sendResponse(client.getSocketFD(), RPL_YOURHOST(server.getServerName(), client.getNickname()));
                server.sendResponse(client.getSocketFD(), RPL_CREATED(server.getServerName(), server.getTime()));
                server.sendResponse(client.getSocketFD(), RPL_MYINFO(server.getServerName(), client.getNickname()));
            }
        }
        return true;
	}
    else if (commandName == "PASS") {
		std::cout << "Processing PASS command  en commandName = PASS: " << command << std::endl;  // Verificar comando recibido
        return handlePassCommand(client, tokens, server);
    }
	else if (commandName == "USER") {
		std::cout << "Processing USER command: " << command << std::endl;  // Verificar comando recibido
		bool result = handleUserCommand(client, tokens, server);
		if (result) {
			std::string welcomeMessage = RPL_WELCOME(client.getNickname(), server.getServerName(), client.getHostname());
			std::cout << "Sending to client: " << welcomeMessage << std::endl;
	        server.sendResponse(client.getSocketFD(), welcomeMessage);

	        std::string yourHostMessage = RPL_YOURHOST(server.getServerName(), client.getNickname());
	        std::cout << "Sending to client: " << yourHostMessage << std::endl;
	        server.sendResponse(client.getSocketFD(), yourHostMessage);

	        std::string createdMessage = RPL_CREATED(server.getServerName(), server.getTime());
	        std::cout << "Sending to client: " << createdMessage << std::endl;
	        server.sendResponse(client.getSocketFD(), createdMessage);

	        std::string myInfoMessage = RPL_MYINFO(server.getServerName(), client.getNickname());
	        std::cout << "Sending to client: " << myInfoMessage << std::endl;
	        server.sendResponse(client.getSocketFD(), myInfoMessage);
		}
		return (result);

		/*
		if (handleUserCommand(client, tokens, server)) {
			std::cout << "Processing USER command  en commandName = USER: " << command << std::endl;
            if (client.isRegistered()) {
				std::string welcomeMessage = RPL_WELCOME(client.getNickname(), server.getServerName(), client.getHostname());
				std::cout << "Sending to client: " << welcomeMessage << std::endl;  // Imprimir el mensaje en la consola
				server.sendResponse(client.getSocketFD(), welcomeMessage);
			//	server.sendResponse(client.getSocketFD(), RPL_WELCOME(client.getNickname(), server.getServerName(), client.getHostname()));
				server.sendResponse(client.getSocketFD(), RPL_YOURHOST(server.getServerName(), client.getNickname()));
				server.sendResponse(client.getSocketFD(), RPL_CREATED(server.getServerName(), server.getTime()));
				server.sendResponse(client.getSocketFD(), RPL_MYINFO(server.getServerName(), client.getNickname()));
			}
		}*/
//        return handleUserCommand(client, tokens, server);
	}


/*************************
    } else if (commandName == "PRIVMSG") {
        return handlePrivMsgCommand(client, tokens, server);
    } else if (commandName == "JOIN") {
        return handleJoinCommand(client, tokens, server);
    } else if (commandName == "PART") {
        return handlePartCommand(client, tokens, server);
    } else if (commandName == "QUIT") {
        return handleQuitCommand(client, tokens, server);
    } else if (commandName == "PING") {
        return handlePingCommand(client, tokens, server);
    } else if (commandName == "PONG") {
        return handlePongCommand(client, tokens, server);
    } else if (commandName == "NOTICE") {
        return handleNoticeCommand(client, tokens, server);
    } else if (commandName == "TOPIC") {
        return handleTopicCommand(client, tokens, server);
    } else if (commandName == "INVITE") {
        return handleInviteCommand(client, tokens, server);
    } else if (commandName == "KICK") {
        return handleKickCommand(client, tokens, server);
    } else if (commandName == "MODE") {
        return handleModeCommand(client, tokens, server);
    } else {
        server.sendResponse(client.getSocketFD(), ERR_UNKNOWNCOMMAND(commandName));
        return false;
    }
***********************/
	server.sendResponse(client.getSocketFD(), ERR_UNKNOWNCOMMAND(commandName));
	return (false);
}




