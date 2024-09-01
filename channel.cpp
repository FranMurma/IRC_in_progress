/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frmurcia <frmurcia@42barcelona.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/01 18:27:04 by frmurcia          #+#    #+#             */
/*   Updated: 2024/09/01 19:00:44 by frmurcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// channel.cpp
#include "server.hpp"

Channel::Channel(const std::string& channel_name) : name(channel_name) {
		std::cout << "Channel " << name << " created." << std::endl;
}

void	Channel::addParticipant(Client* client) {
		if (std::find(participants.begin(), participants.end(), client) == participants.end()) {
				participants.push_back(client);
				std::cout << client->getNickname() << " joined channel " << name << std::endl;
		}
		else {
				std::cout << client->getNickname() << " already in channel " << name << std::endl;
		}
}

void Channel::removeParticipant(Client* client) {
    participants.erase(std::remove(participants.begin(), participants.end(), client), participants.end());
}

const	std::vector<Client*>& Channel::getParticipants() const {
    return (participants);
}

const std::string& Channel::getName() const {
    return (name);
}

