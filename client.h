#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

class Client{
public:
	int status;
	int client_socket;
	char client_buffer[BUFFER_SIZE];
	
	int server_port;
	const char *server_address;
	struct sockaddr_in server_sock_addr;
	
	Client();
	void fill_server_details(int, const char*);
	void connect_with_server(int);
	void read_data();
	void write_data();	
	~Client();		
};
