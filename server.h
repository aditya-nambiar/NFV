#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

class Server{
	public:
		int server_port;
		const char *server_address;
		int server_socket;
		struct sockaddr_in server_sock_addr;
		char server_buffer[BUFFER_SIZE];
		
		int client_socket[MAX_CONNECTIONS];
		struct sockaddr_in client_sock_addr;
		socklen_t client_addr_len; 
		pthread_t tid[MAX_CONNECTIONS];
	
		Server(int,const char*);
		void start_listening();
		void accept_connections(void*(*multithreading_func)(void*));
		void read_data();
		void write_data();	
		~Server();
};
