#include "channel.hpp"
#include "server.hpp"
#include "client.hpp"
#include "commands.hpp"
#include "responses.hpp"
#include "utils.hpp"
#include "pongCommand.hpp"
#include <iostream>


void handlePongCommand(Client& client, const std::vector<std::string>& tokens, Server& server) {
	(void)server;
    if (tokens.size() < 2) {
        return;  // Si el PONG no tiene parámetros, lo ignoramos
    }
    std::cout << "Recibido PONG del cliente " << client.getNickname() << std::endl;
    // Aquí puedes procesar el PONG si es necesario. En muchos casos, simplemente es confirmación de que el cliente está activo.
}

