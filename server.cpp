#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <csignal>
#include "server.hpp" // Asegurarse de que el encabezado esté correctamente incluido
#include "socketUtils.hpp"

//Volatile para que no cachee sino que la consulte siempre
//sig_atomic_t tipo de C y CPP para variables que pueden modificadarse en un manejador de señales,
//ya que es un tipo atomico, cualquier operacion se hara de una forma indivisible (un golpe)
//stop_server lo vamos a usar como flag para ver cuando para el servidor
volatile sig_atomic_t stop_server = 0;

// Definición del manejador de señales
void	signalHandler(int signal) {
	(void)signal;  // Ignorar el parámetro para evitar advertencias
	stop_server = 1;  // Cambiar el estado para indicar que el servidor debe detenerse
}

Server::Server(void) {
	this->serverName = "fran_server";
	this->setTime();
}

// Método de configuración del manejador de señales en la clase Server
void	Server::setupSignalHandler() {
	std::signal(SIGINT, signalHandler);
}


void	Server::configureServer(int port, std::string password) {
	this->password = password;  // Asignar la contraseña
	this->port = port;          // Asignar el puerto

	socket_fd = createSocket();//Creamos un socket 
	enableReuseAddress(socket_fd);// Opcion de reusar rapidamente la direccion
	setNonBlockingMode(socket_fd);// Que no nos bloquee
	bindSocket(socket_fd, port);//Vinculamos el socket a la IP y puerto
	listenOnSocket(socket_fd);// Lo ponemos en escucha

	// Añadir el socket a la lista de descriptores supervisados por poll (abajo: sensible a POLLIN)
	addPollFd(poll_fds, socket_fd, POLLIN); //Añadir el socket_fd, a los poll_fds

	setupSignalHandler();      // Configurar el manejador de señales
}

void Server::run() {
    setupSignalHandler();  // Configurar el manejador de señales

    while (!stop_server) {  // Repetir mientras no se haya recibido la señal de interrupción
        std::cout << "Currently monitoring " << (poll_fds.size() - 1) << " clients." << std::endl;
        pollSockets();  // Realizar poll para supervisar los sockets
        handlePollEvents();  // Manejar los eventos de poll
    }

    cleanup();  // Limpieza de recursos al finalizar
}


// Supervisamos los sockets utilizando la llamada al sistema poll()
void Server::pollSockets() {
// poll_fds.data(): Devuelve un puntero al primer elemento del vector poll_fds, 
// que contiene estructuras pollfd para cada socket que el servidor está supervisando.
// -1: Especifica que poll() debe esperar indefinidamente hasta que ocurra un evento 
// en uno de los descriptores de archivo supervisados.
    int poll_count = poll(poll_fds.data(), poll_fds.size(), -1); //el poll_fds

    if (poll_count < 0) {// Error!
        if (stop_server) {  // Si la señal fue recibida, salir del bucle sin error
            return;
        }
        std::cout << "Error: Poll failed." << std::endl;
        stop_server = 1;  // Señal para detener el servidor en caso de error
    }
}

// Encabezado de la función para manejar los eventos de poll
void Server::handlePollEvents() {
    for (size_t i = 0; i < poll_fds.size(); ++i) {
        if (poll_fds[i].revents & POLLIN) {  // Hay datos disponibles para leer
            if (poll_fds[i].fd == socket_server_fd) {  // Actividad en el socket del servidor
                acceptClient();  // Aceptar nueva conexión de cliente
            } else {  // Actividad en un socket de cliente
                handleClient(poll_fds[i].fd);  // Manejar datos del cliente
            }
        }
    }
}

// Encabezado de la función para limpiar recursos al finalizar
void Server::cleanup() {
    std::cout << "Signal detected, cleaning up..." << std::endl;
    for (size_t i = 0; i < poll_fds.size(); ++i) {
        close(poll_fds[i].fd);  // Cerrar cada socket supervisado
    }
    poll_fds.clear();  // Limpiar el vector de poll_fds

    // Eliminar todos los clientes
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end();) {
        delete it->second;  // Liberar la memoria del cliente
        it = clients.erase(it);  // Eliminar el cliente del mapa y avanzar el iterador
    }

    close(socket_server_fd);  // Cerrar el socket del servidor
    std::cout << "Server shutdown completed." << std::endl;
}



