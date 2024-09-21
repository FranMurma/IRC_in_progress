#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"

Channel::Channel(const std::string& channel_name, Server& srv) : name(channel_name), server(srv) {
	std::cout << "Canal creado: " << name << std::endl;
}

void	Channel::manageUser(Client* client, UserRole role, bool add) {
    std::vector<Client*>* list = NULL;

    switch (role) {
        case PARTICIPANT: list = &participants; break;
        case OPERATOR: list = &operators; break;
        case INVITED: list = &invited; break;
    }

    if (list) {
        if (add) {
            if (std::find(list->begin(), list->end(), client) == list->end()) {
                list->push_back(client);
                std::cout << client->getNickname() << " añadido como " << role << std::endl;
            }
        } else {
            list->erase(std::remove(list->begin(), list->end(), client), list->end());
            std::cout << client->getNickname() << " removido de " << role << std::endl;
        }
    }
}


    // Función para obtener el nombre del canal
const	std::string& Channel::getName() const {
		return (name);
}

    // Función para obtener la lista de participantes
const	std::vector<Client*>& Channel::getParticipants() const {
		return (participants);
}

const	std::vector<Client*>& Channel::getOperators() const {
	return (operators);
}

const	std::vector<Client*>& Channel::getInvited() const {
	return (invited);
}

void	Channel::dummyUseServer() {
    std::cout << &server << std::endl;
}

std::set<int> Channel::getClientFDs() const {
    std::set<int> fds;
    for (size_t i = 0; i < participants.size(); ++i) {
        fds.insert(participants[i]->getSocketFD());
    }
    return fds;
}

