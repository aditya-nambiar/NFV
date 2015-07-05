#include "server.h"

int g_reuse = 1;

Server::Server(int server_port, const char *server_address){
	this->server_port = server_port;
	this->server_address = server_address;
	status = 0;
	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	report_error(server_socket);
	bzero((char *) &server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;  	
	server_sock_addr.sin_port = htons(server_port); 
	inet_aton(server_address, &server_sock_addr.sin_addr);
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));
	signal(SIGPIPE, SIG_IGN);	  
	status = bind(server_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);	
}

void Server::listen_accept(void*(*multithreading_func)(void*)){
	int i;
	i=0;
	while(1){
		bzero(server_buffer, BUFFER_SIZE);
		status = recvfrom(server_socket, server_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&clients[i].client_sock_addr, &g_addr_len);
		memcpy(&clients[i].num, server_buffer, sizeof(int)); 		
		report_error(status);
		status = pthread_create(&tid[i], NULL, multithreading_func, &clients[i]);
		report_error(status);
		cout<<"Server side: Connection made with Client"<<clients[i].num<<endl;
		i++;
	}
}

void Server::connect_with_client(){
	memcpy(server_buffer, &server_port, sizeof(int));
	memcpy(server_buffer+sizeof(int), server_address, strlen(server_address));
	write_data();
}

void Server::read_data(){
	bzero(server_buffer, BUFFER_SIZE);
	status = recvfrom(server_socket, server_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
	check_conn(status);
	report_error(status);
}

void Server::write_data(){
	status = sendto(server_socket, server_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);
}

Server::~Server(){
	close(server_socket);
}
