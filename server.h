#ifndef SERVER_H
#define SERVER_H

#include "utils.h"
#include "packet.h"

extern int g_reuse;

extern int g_total_threads;
extern queue<ClientDetails> g_connections;

extern pthread_mutex_t g_queue_lock = PTHREAD_MUTEX_INITIALIZER;
extern pthread_cond_t g_enqueue = PTHREAD_COND_INITIALIZER;
extern pthread_cond_t g_queue_full = PTHREAD_COND_INITIALIZER;

ClientDetails fetch_connection();

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
	ClientDetails clients[MAX_CONNECTIONS];
	pthread_t tid[MAX_CONNECTIONS];
	
	Server();
	void fill_server_details(int, const char*);
	void bind_server();
	void listen_accept(void*(*multithreading_func)(void*));
	void connect_with_client();
	void read_data();
	void write_data();	
	~Server();
};

#endif //SERVER_H