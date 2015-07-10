#include "packet.h"

class RawClient{
public:
	static const int on = 1;
	static const char *interface;
	static struct ifreq ifr;
	static void set_interface(const char*);

	int status;
	int client_port;
	int client_socket;
	char *client_address;
	struct sockaddr_in client_sock_addr;

	int server_port;
	char *server_address;
	struct sockaddr_in server_sock_addr;

	RawClient();
	void fill_client_details(int, const char*);
	void bind_client();
	void fill_server_details(int, const char*);
	void connect_with_server(Packet&, int);
	void read_data(Packet&);
	void write_data(Packet&);	
	~RawClient();		
};