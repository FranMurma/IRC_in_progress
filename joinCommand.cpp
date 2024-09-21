#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "responses.hpp"
#include "utils.hpp"
#include <iostream>

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


