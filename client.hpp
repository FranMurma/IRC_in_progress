#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <unistd.h>

class	Client {
private:
	int	client_fd;  // Descriptor de archivo del cliente
	std::string nickname;
	std::string input_buffer;  // Buffer temporal para almacenar datos recibidos

public:
	// Constructor que inicializa el descriptor de archivo del cliente
	Client(int fd);

	int	getFd() const;  // Método para obtener el fd
  std::string getNickname() const;  // Método para obtener el apodo
  void	setNickname(const std::string& new_nick);  // Método para establecer el apodo
	std::string& getInputBuffer();
	void	setInputBuffer(const std::string& buffer);
	void	appendToBuffer(const std::string& data);
  bool	hasCompleteCommand(std::string& command);
    // Destructor
	~Client();
};

#endif // CLIENT_HPP

