
#include <iostream>
#include "utils.hpp"
#include "server.hpp"
#include "client.hpp"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return (0);
    }
    // Verificar que el puerto es un número válido
    if ((!is_valid_number(argv[1])) || (!is_valid_port(argv[1]))) {
        std::cerr << "Invalid port number." << std::endl;
        return (0);
    }
    // Convertir el puerto a entero
    int port = std::atoi(argv[1]);
		std::string password = argv[2];

    // Crear la instancia del servidor
    Server server(port, password);
		server.run();

    // Aquí podrías agregar más lógica para iniciar el servidor
    // server.start() o server.listen() según lo que vayas implementando
    return (0);
}
