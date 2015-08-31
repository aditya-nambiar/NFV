#ifndef SERVER_H
#define SERVER_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"

class Server{
public:
	int status;
	int server_port;
	char *server_addr;
	int server_socket;
	struct sockaddr_in server_sock_addr;
	Packet pkt;
	
	int client_num;
	struct sockaddr_in client_sock_addr;
	vector<ClientDetails> clients;
	vector<pthread_t> tid;
	
	struct ThreadPool tpool;

	Server();
	void begin_thread_pool(int, void*(*thread_func)(void*));
	void set_total_connections(int);
	void fill_server_details(int, const char*);
	void bind_server();
	void listen_accept();
	void listen_accept(void*(*multithreading_func)(void*));	
	void connect_with_client();
	void read_data();
	void write_data();	
	~Server();
};

#endif //SERVER_H