#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

class Client{
	public:
		int client_socket;
		char client_buffer[BUFFER_SIZE];
		
		int server_port;
		const char *server_address;
		struct sockaddr_in server_sock_addr;
		
		Client();
		void input_server_details(int, const char*);
		void connect_with_server();
		void read_data();
		void write_data();	
		~Client();		
};
