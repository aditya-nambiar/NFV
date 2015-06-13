#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

class SERVER{
	public:
		int server_port;
		const char *server_address;
		int server_socket;
		struct sockaddr_in server_addr;
		char server_buffer[BUFFER_SIZE];
		
		int client_socket[MAX_CONNECTIONS];
		struct sockaddr_in client_addr;
		socklen_t client_addr_len; 
		pthread_t tid[MAX_CONNECTIONS];
	
		SERVER(int,const char*);
		void start_listening();
		void accept_connections(void*(*multithreading_func)(void*));
		~SERVER();
};
