#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>

#include <pthread.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
using namespace std;

#define MAX_CONNECTIONS 100

class SERVER{
	public:
		int listen_port;
		int listen_socket;
		int client_socket[MAX_CONNECTIONS];
		pthread_t tid[MAX_CONNECTIONS];
		struct sockaddr_in server_addr;
		struct sockaddr_in client_addr;
		socklen_t client_addr_len; 
	
		SERVER();
		void start_listening();
		void accept_connections();
		void* multithreading_func(void*);
		void report_error(int);
		void print_message(const char*);
		~SERVER();
};
