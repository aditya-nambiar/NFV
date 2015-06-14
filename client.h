#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

class CLIENT{
	public:
		int client_socket;
		char client_buffer[BUFFER_SIZE];
		
		int server_port;
		const char *server_address;
		struct sockaddr_in server_addr;
		
		CLIENT();
		void input_server_details(int,const char*);
		void connect_with_server();
		const char* read_data();
		void write_data(const char*);	
		~CLIENT();		
};
