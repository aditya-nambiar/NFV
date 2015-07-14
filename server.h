#ifndef SERVER_H
#define SERVER_H

#include "utils.h"
#include "packet.h"

extern int g_reuse;

class Server{
public:
	int status;
	int server_port;
	const char *server_address;
	int server_socket;
	struct sockaddr_in server_sock_addr;
	char server_buffer[BUFFER_SIZE];
	
	struct sockaddr_in client_sock_addr;
	ClientDetails clients[MAX_CONNECTIONS];
	pthread_t tid[MAX_CONNECTIONS];
	
	Server(int, const char*);
	void listen_accept(void*(*multithreading_func)(void*));
	void connect_with_client();
	void read_data();
	void write_data();	
	~Server();
};

#endif //SERVER_H