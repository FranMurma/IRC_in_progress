#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "client.hpp"



enum UserRole { PARTICIPANT, OPERATOR, INVITED };

class Channel {

	public :
		// Constructor
		Channel(const std::string& channel_name, Server& srv);

		//Anadir y eliminar usuarios de todo tipo al canal
		void	manageUser(Client* client, UserRole role, bool add);

	    // Función para obtener el nombre del canal
		const	std::string& getName() const;

	    // Función para obtener la lista de participantes
		const	std::vector<Client*>& getParticipants() const;

		const	std::vector<Client*>& getOperators() const;
		const	std::vector<Client*>& getInvited() const;
		void	dummyUseServer();
		const	std::string& getTopic() const { return topic; }
		void	setTopic(const std::string& newTopic) { topic = newTopic; }
		std::set<int>getClientFDs() const;


	private:
		std::string name;
		std::string topic;
		std::vector<Client*> participants;
		std::vector<Client*> operators;
		std::vector<Client*> invited;
		Server& server;


};

#endif // CHANNEL_HPP
