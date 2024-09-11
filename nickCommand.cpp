// nickCommand.cpp
#include "commands.hpp"
#include "responses.hpp"
#include "server.hpp"
#include "utils.hpp"

// Función auxiliar para validar si el nickname contiene caracteres válidos
static bool isInvalidNick(const std::string& nick) {
    std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-[]\\`^{}|_";
    return nick.empty() || nick.find_first_not_of(validChars) != std::string::npos;
}

bool handleNickCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NONICKNAMEGIVEN());
        return false;
    }
    
    std::string nickname = tokens[1];

    // Validar si el nickname contiene caracteres no permitidos
    if (isInvalidNick(nickname)) {
        server.sendResponse(client.getSocketFD(), ERR_ERRONEUSNICKNAME(nickname));
        return false;
    }

    // Verificar si el nickname ya está en uso
    if (isNicknameInUse(nickname)) {
        server.sendResponse(client.getSocketFD(), ERR_NICKNAMEINUSE(nickname));
        return false;
    }

    // Si el cliente ya está registrado, notificar el cambio de nick a otros usuarios
    if (client.isRegistered()) {
        std::string oldNickname = client.getNickname();
        client.setNickname(nickname);

        std::string message = ":" + oldNickname + "!" + client.getUsername() + "@" + client.getHostname() + " NICK :" + nickname;
        server.broadcastMessage(message);
    } 
    else {
        // Si no está registrado completamente, actualizar el nick y verificar si ya puede ser registrado
        client.setNickname(nickname);
        if (!client.getUsername().empty() && client.getHasPassword()) {
            client.setRegistered(true);
            server.sendResponse(client.getSocketFD(), RPL_WELCOME(client.getNickname(), server.getServerName(), client.getHostname()));
            server.sendResponse(client.getSocketFD(), RPL_YOURHOST(server.getServerName(), client.getNickname()));
            server.sendResponse(client.getSocketFD(), RPL_CREATED(server.getServerName(), server.getTime()));
            server.sendResponse(client.getSocketFD(), RPL_MYINFO(server.getServerName(), client.getNickname()));
        }
    }
    return true;
}

