/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: frmurcia <frmurcia@42barcelona.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/01 18:16:41 by frmurcia          #+#    #+#             */
/*   Updated: 2024/09/01 19:10:48 by frmurcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// channel.hpp
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "client.hpp"

class	Client; // Declaración adelantada de Client

class	Channel {
	private:
		std::string name;
    std::vector<Client*> participants;

	public:
    Channel(const std::string& channel_name);
    void	addParticipant(Client* client);
    void	removeParticipant(Client* client);
    const	std::vector<Client*>& getParticipants() const;
    const	std::string& getName() const;
};

#endif // CHANNEL_HPP

