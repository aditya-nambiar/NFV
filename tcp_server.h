#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "utils.h"
#include "packet.h"

class TCPServer{
public:
	int status;
	int server_port;
	char *server_addr;
	int server_socket;
	struct sockaddr_in server_sock_addr;
	Packet pkt;
	
	int client_socket[MAX_CONNECTIONS];
	struct sockaddr_in client_sock_addr;
	socklen_t client_addr_len; 
	pthread_t tid[MAX_CONNECTIONS];

	TCPServer();
	void fill_server_details(int, const char*);
	void bind_server();
	void listen_accept(void*(*multithreading_func)(void*));
	void read_data();
	void write_data();	
	~TCPServer();
};

#endif //TCP_SERVER_H