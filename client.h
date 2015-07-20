#ifndef CLIENT_H
#define CLIENT_H

#include "utils.h"
#include "packet.h"

class Client{
public:
	int status;
	int client_socket;
	Packet pkt;
	
	int server_port;
	char *server_addr;
	struct sockaddr_in server_sock_addr;
	
	Client();
	void fill_server_details(int, const char*);
	void connect_with_server(int);
	void read_data();
	void write_data();	
	~Client();		
};

#endif //CLIENT_H