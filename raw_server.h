#include "packet.h"

extern int g_reuse;

class RawServer{
public:
	static const int on = 1;
	static const char *interface;
	static struct ifreq ifr;
	static void set_interface(const char*);

	int status;
	int server_port;
	char *server_address;
	int server_socket;
	struct sockaddr_in server_sock_addr;

	struct sockaddr_in client_sock_addr;
	ClientDetails clients[MAX_CONNECTIONS];
	pthread_t tid[MAX_CONNECTIONS];

	Server();
	void fill_server_details(int, const char*);
	void bind_server();
	void listen_accept(void*(*multithreading_func)(void*), Packet);
	void connect_with_client(Packet);
	void read_data(Packet);
	void write_data(Packet);	
	~Server();
};
