#include "utils.hpp"
#include "server.hpp"
#include "responses.hpp"
#include <iomanip> // Añade esta línea para usar std::setw y std::hex
#include <cstdlib>  // Para usar atoi


// Declaración de la lista de comandos válidos
std::set<std::string> initializeValidCommands() {
    std::set<std::string> commands;
    commands.insert("PASS");
    commands.insert("NICK");
    commands.insert("USER");
    commands.insert("JOIN");
    commands.insert("PART");
    commands.insert("QUIT");
    commands.insert("PRIVMSG");
    return commands;
}

const std::set<std::string> validCommands = initializeValidCommands();


bool	is_valid_number(const char* str) {
	while (*str) {
		if (!isdigit(*str)) {
			return (false);
		}
		str++;
	}
	return (true);
}

bool	is_valid_port(const char* argument) {
	int	port = atoi(argument);
	if (port < 1 || port > 65535)
		return (false);
	return (true);
}


// Funcion para convertir los \n a \r\n
std::string convertToCRLF(const std::string& input) {
    std::string result;
    result.reserve(input.size());  // Reservar espacio para evitar múltiples realocaciones
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\n') {
            if (i == 0 || input[i - 1] != '\r') {
                result += '\r';  // Añadir \r antes de \n si no está presente
            }
        }
        result += input[i];  // Añadir el carácter actual
    }
	std::cout << "Aplicamos CRLF" << std::endl;
    return result;
}


// Función para dividir una cadena en comandos usando un delimitador
std::vector<std::string> splitString(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        std::string token = str.substr(start, end - start);
        if (!token.empty()) {  // Ignora los tokens vacíos
            tokens.push_back(token);
        }
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    // Agregar la última parte si no está vacía
    std::string token = str.substr(start);
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}


// Implementación de processCommand
bool processCommand(const std::string& command, int client_fd, Server& server) {
    std::vector<std::string> tokens = splitString(command, " ");
    if (tokens.empty()) return false;

    std::string commandName = tokens[0];
    
    // Imprimir el comando recibido
    std::cout << "Received command: " << command << std::endl;
    std::cout << "Hex dump: ";
    for (size_t i = 0; i < command.size(); ++i) {
        char c = command[i];
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c << " ";
    }
    std::cout << std::endl;

    if (commandName == "CAP") {
        if (tokens.size() >= 2 && tokens[1] == "LS") {
            server.sendResponse(client_fd, "CAP * LS :multi-prefix");
        } else {
			server.sendResponse(client_fd, "CAP * NAK");
        }
        return true;
    }

    if (validCommands.find(commandName) == validCommands.end()) {
        server.sendResponse(client_fd, "421 ERR_UNKNOWNCOMMAND :Unknown command");
        return true;
    }

    if (commandName == "PASS") {
        if (tokens.size() < 2) {
            server.sendResponse(client_fd, ERR_NEEDMOREPARAMS("PASS"));
            return true;
        }
        server.sendResponse(client_fd, "NOTICE * :Password accepted");
        return true;


		
    } else if (commandName == "NICK") {
        if (tokens.size() < 2) {
            server.sendResponse(client_fd, ERR_NONICKNAMEGIVEN());
            return true;
        }
		std::string nickname = tokens[1];
		// Verificar si el nickname es válido
		if (!isValidNickname(nickname)) { // Esta función FALTA!!!
        server.sendResponse(client_fd, ERR_ERRONEUSNICKNAME(nickname));
        return true;
		}
		// Verifica si el nickname está en uso
		if (isNicknameInUse(nickname)) {
			server.sendResponse(client_fd, ERR_NICKNAMEINUSE(nickname));
			return (true);
		}
		server.sendResponse(client_fd, "NOTICE * :Nickname set to " + nickname);
        return true;



    } else if (commandName == "USER") {
        if (tokens.size() < 5) {
            server.sendResponse(client_fd, "461 ERR_NEEDMOREPARAMS :Not enough parameters for USER");
            return true;
        }
        // Completar la autenticación y enviar las respuestas necesarias
        std::string nick = tokens[1]; // Usar el nickname almacenado si es necesario
        server.sendResponse(client_fd, "001 " + nick + " :Welcome to the IRC server");
        server.sendResponse(client_fd, "002 " + nick + " :Your host is fran_server, running version 1.0");
        server.sendResponse(client_fd, "003 " + nick + " :This server was created now");
        server.sendResponse(client_fd, "004 " + nick + " fran_server 1.0 iowghraAsORTVS");
        return true;
    }

    // Otros comandos se manejan aquí

    return false; // Para cualquier texto no comando o desconocido
}

bool isValidNickname(const std::string& nickname) {
    std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-[]\\`^{}|_";
    return !nickname.empty() && nickname.find_first_not_of(validChars) == std::string::npos;
}

bool isNicknameInUse(const std::string& nickname) {
    (void)nickname; // Para evitar la advertencia de parámetro no usado
    // Lógica para verificar si el nickname está en uso
    return false;
}


/************************
// Función auxiliar para enviar respuestas al cliente
void sendResponse(int client_fd, const std::string& response) {
    std::string response_with_crlf = convertToCRLF(response); // Asegura \r\n
    if (send(client_fd, response_with_crlf.c_str(), response_with_crlf.size(), 0) == -1) {
        std::cout << "Error: Failed to send response to client." << std::endl;
    }
}


// Funciones adicionales para verificar estado de nicknames y usuarios (ejemplos)
bool isNicknameInUse(const std::string& nickname) {
	(void)nickname; // Para evitar la advertencia de parámetro no usado
    // Lógica para verificar si el nickname está en uso
    return false;
}
*****************/

bool isUserOrChannelExists(const std::string& target) {
	(void)target; // Para evitar la advertencia de parámetro no usado
    // Lógica para verificar si el usuario o canal existe
    return true;
}

