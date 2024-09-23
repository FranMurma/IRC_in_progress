#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "responses.hpp"
#include "utils.hpp"
#include <iostream>


/*
void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("JOIN"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        channel = new Channel(channelName, server); // Crear un nuevo canal
        server.addChannel(channel);
    }
	std::cout << "Hostname for client: " << client.getHostname() << std::endl;
	std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN :" + channelName;


    channel->manageUser(&client, PARTICIPANT, true);
	// Enviar el mensaje de JOIN al propio client
	server.sendResponse(client.getSocketFD(), joinMsg);

    // Notificar a otros miembros del canal que alguien ha entrado
	std::set<int> exclude_fds;
    exclude_fds.insert(client.getSocketFD());
//    server.broadcastMessage(joinMsg, std::set<int>(), exclude_fds);

	// Enviar a los otros clientes en el canal
	server.broadcastMessage(joinMsg, channel->getClientFDs(), exclude_fds);

    // Enviar el tópico del canal
    if (!channel->getTopic().empty()) {
        server.sendResponse(client.getSocketFD(), RPL_TOPIC(server.getServerName(), channelName, channel->getTopic()));
    } else {
        server.sendResponse(client.getSocketFD(), RPL_NOTOPIC(server.getServerName(), channelName));
    }

    // Preparar y enviar la lista de nombres de los miembros del canal
    std::string clientList;
    const std::vector<Client*>& participants = channel->getParticipants();
    for (size_t i = 0; i < participants.size(); ++i) {
        clientList += participants[i]->getNickname();
        if (i < participants.size() - 1) {
            clientList += " ";
        }
    }

    server.sendResponse(client.getSocketFD(), RPL_NAMREPLY(server.getServerName(), client.getNickname(), channelName, clientList));
	server.sendResponse(client.getSocketFD(), RPL_ENDOFNAMES(client.getNickname(), channelName));
	std::string mode = "+nt"; // Ajusta según el modo real del canal
	server.sendResponse(client.getSocketFD(), RPL_CHANNELMODEIS(server.getServerName(), client.getNickname(), channelName, mode));

    std::cout << client.getNickname() << " joined channel " << channelName << std::endl;
}

*****************/
void handleJoinCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
    if (tokens.size() < 2) {
        server.sendResponse(client.getSocketFD(), ERR_NEEDMOREPARAMS("JOIN"));
        return;
    }

    std::string channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        // Crear un nuevo canal si no existe
        channel = new Channel(channelName, server);
        server.addChannel(channel);
    }

    // Mensaje de confirmación del JOIN para todos
    std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + 
                          "@" + client.getHostname() + " JOIN :" + channelName;

    // Enviar el mensaje de JOIN al cliente que se unió
    server.sendResponse(client.getSocketFD(), joinMsg);

    // Añadir el cliente al canal como participante
    channel->manageUser(&client, PARTICIPANT, true);

    // Notificar a los demás miembros del canal sobre el nuevo usuario
    std::set<int> exclude_fds;
    exclude_fds.insert(client.getSocketFD());
    server.broadcastMessage(joinMsg, channel->getClientFDs(), exclude_fds);

    // Enviar la lista completa de mensajes (tópico, lista de usuarios, modos) a todos los usuarios del canal
    const std::vector<Client*>& participants = channel->getParticipants();
    std::string clientList;
    for (size_t i = 0; i < participants.size(); ++i) {
        clientList += participants[i]->getNickname();
        if (i < participants.size() - 1) {
            clientList += " ";
        }
    }

    // Mensajes que deben enviarse a todos los clientes en el canal
    std::string notopicMsg = ":" + server.getServerName() + " 331 " + channelName + " :No topic is set";
    std::string namesReplyMsg = ":" + server.getServerName() + " 353 " + client.getNickname() + " = " + channelName + " :" + clientList;
    std::string endOfNamesMsg = ":" + server.getServerName() + " 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list";
    std::string modeMsg = ":" + server.getServerName() + " 324 " + client.getNickname() + " " + channelName + " +nt"; // Ajusta los modos según sea necesario

    // Enviar los mensajes a todos los clientes en el canal sin excluir a nadie
    std::set<int> empty_exclude_fds;  // Un conjunto vacío para no excluir a nadie
    server.broadcastMessage(notopicMsg, channel->getClientFDs(), empty_exclude_fds);
    server.broadcastMessage(namesReplyMsg, channel->getClientFDs(), empty_exclude_fds);
    server.broadcastMessage(endOfNamesMsg, channel->getClientFDs(), empty_exclude_fds);
    server.broadcastMessage(modeMsg, channel->getClientFDs(), empty_exclude_fds);

    std::cout << client.getNickname() << " joined channel " << channelName << std::endl;
}

