#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "utils.h"
#include "packet.h"

class TCPClient{
public:
	int status;
	int client_socket;
	int client_port;
	char *client_addr;
	struct sockaddr_in client_sock_addr;
	Packet pkt;
	
	int server_port;
	char *server_addr;
	struct sockaddr_in server_sock_addr;
	
	TCPClient();
	void fill_client_details(const char*);
	void bind_client();
	void fill_server_details(int, const char*);
	void connect_with_server();
	void read_data();
	void write_data();
	~TCPClient();
};

#endif //TCP_CLIENT_H