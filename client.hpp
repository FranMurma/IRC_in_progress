#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>

class	Client {

	private:
		int		client_fd;  // Descriptor de archivo del cliente
		std::string nickname;
		std::string input_buffer;  // Buffer temporal para almacenar datos recibidos
		bool	finished_line;        // Indica si hay una línea completa para procesar
		bool	authenticated;  // Atributo para la autenticación
		bool	received_welcome;  // Indica si el cliente ha recibido la bienvenida

		std::string username;
		std::string hostname;
		std::string servername;
		std::string realname;
		bool	passReceived;
		bool	nickReceived;
		bool	userReceived;



	public:
	// Constructor que inicializa el descriptor de archivo del cliente
		Client(int fd);
		int		getFd() const;  // Método para obtener el fd
	  const std::string getNickname() const;  // Método para obtener el apodo
	  void	setNickname(const std::string& new_nick);  // Método para establecer el apodo
		const std::string& getInputBuffer() const;
		void	setInputBuffer(const std::string& buffer);
		void	appendToBuffer(const std::string& data);
	  bool	hasCompleteCommand(std::string& command);
		bool	isFinishedLine() const;
		void	clearFinishedLine();
		bool	isAuthenticated() const;   // Método para verificar si está autenticado
		void	setAuthenticated(bool status);  // Método para establecer el estado de autenticación
		void	setUsername(const std::string& user);
    void	setHostname(const std::string& host);
    void	setServername(const std::string& server);
    void	setRealname(const std::string& real);
		void	setPassReceived(bool status);
    bool	getPassReceived() const;
    void	setNickReceived(bool status);
    bool	getNickReceived() const;
    void	setUserReceived(bool status);
    bool	getUserReceived() const;
		bool	hasReceivedWelcome() const;
    void	setReceivedWelcome(bool value);

		void	clearBuffer();
    // Destructor
		~Client();
};

#endif // CLIENT_HPP

