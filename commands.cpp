#include "commands.hpp"
#include "server.hpp"
#include "client.hpp"
#include <iostream>
#include <algorithm> // Para std::find_if

#include <cstring> // Para std::strlen

void Command::handleNickCommand(Server& server, int client_fd, const std::string& new_nick) {
    // 1. Validar el nuevo apodo
    if (new_nick.empty() || new_nick.length() > 30) {
        std::string error_msg = "ERROR: Invalid nickname.\n";
        send(client_fd, error_msg.c_str(), std::strlen(error_msg.c_str()), 0);  // Enviar error al cliente
        return;
    }

    // 2. Verificar si el apodo ya está en uso
    std::map<int, Client*>::const_iterator it;
    for (it = server.getClients().begin(); it != server.getClients().end(); ++it) {
        if (it->second->getNickname() == new_nick) {
            // Apodo ya en uso
            std::string error_msg = "ERROR: Nickname already in use.\n";
            send(client_fd, error_msg.c_str(), std::strlen(error_msg.c_str()), 0);  // Enviar error al cliente
            return;
        }
    }

    // 3. Actualizar el apodo del cliente
    Client* client = server.getClient(client_fd);  // Obtener el objeto Client correspondiente al fd
    if (client) {
        std::string old_nick = client->getNickname();
        client->setNickname(new_nick);  // Actualizar el apodo
        std::cout << "Client (fd " << client_fd << ") changed nickname from " << old_nick << " to " << new_nick << std::endl;

        // 4. Informar al cliente y a los demás usuarios
        std::string success_msg = "Your nickname is now " + new_nick + ".\n";
        send(client_fd, success_msg.c_str(), std::strlen(success_msg.c_str()), 0);  // Confirmar al cliente

        // Informar a los demás clientes que el usuario ha cambiado de apodo
        std::string notification_msg = ":" + old_nick + " NICK :" + new_nick + "\n";
        server.broadcastMessage(notification_msg, client_fd);  // Este método debería enviar el mensaje a todos menos al cliente_fd
    } else {
        // No se pudo encontrar al cliente en el servidor
        std::string error_msg = "ERROR: Client not found.\n";
        send(client_fd, error_msg.c_str(), std::strlen(error_msg.c_str()), 0);  // Enviar error al cliente
    }
}

void Command::handleJoinCommand(Server& server, int client_fd, std::istringstream& ss) {
    std::string channel_name;
    ss >> channel_name;
    if (channel_name.empty()) {
        server.sendMessage(client_fd, "ERROR: No channel specified.\n");
        return;
    }

    // Verificar si el canal ya existe
    Channel* channel = server.getChannel(channel_name);
    if (!channel) {
        // Crear nuevo canal
        channel = new Channel(channel_name);
        server.addChannel(channel);
        server.sendMessage(client_fd, "Channel created: " + channel_name + "\n");
    }
		else {
				// Verificar si el cliente ya está en el canal
				Client* client = server.getClient(client_fd);
				if (std::find(channel->getParticipants().begin(), channel->getParticipants().end(), client) != channel->getParticipants().end()) {
						server.sendMessage(client_fd, "ERROR: Already in channel: " + channel_name + "\n");
						return;
				}
		}

    // Añadir cliente al canal
    Client* client = server.getClient(client_fd);
    channel->addParticipant(client);
    server.sendMessage(client_fd, "Joined channel: " + channel_name + "\n");
}

void Command::handlePrivmsgCommand(Server& server, int client_fd, std::istringstream& ss) {
	 (void)server;   // Evitar el warning por parámetro no utilizado
    (void)client_fd;
    (void)ss;
    // Lógica para manejar el comando PRIVMSG
}

// Implementa el resto de las funciones de comandos como `handleUserCommand`, `handleJoinCommand`, etc.

