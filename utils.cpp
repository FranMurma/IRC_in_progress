#include "utils.hpp"
#include "server.hpp"

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
	
