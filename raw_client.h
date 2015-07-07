#include "packet.h"

class RawClient{
public:
	static const int on = 1;

	int status;
	int client_port;
	int client_socket;
	struct sockaddr_in client_sock_addr;

	int server_port;
	const char *server_address;
	struct sockaddr_in server_sock_addr;

	char *interface;
	struct ifreq ifr;

	Client();
	void fill_server_details(int, const char*);
	void connect_with_server(int);
	void read_data();
	void write_data();	
	~Client();		
};