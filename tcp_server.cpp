#include "tcp_server.h"

int g_reuse = 1;

TCPServer::TCPServer(){
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	report_error(server_socket);
	server_addr = allocate_str_mem(INET_ADDRSTRLEN);
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));
	signal(SIGPIPE, SIG_IGN);	  
}

void TCPServer::fill_server_details(int server_port, const char *server_addr){
	this->server_port = server_port;
	strcpy(this->server_addr, server_addr);
	bzero((char *) &server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;  	
	server_sock_addr.sin_port = htons(server_port);
	status = inet_aton(server_addr, &server_sock_addr.sin_addr);	
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void TCPServer::bind_server(){
	status = bind(server_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);	
}

void TCPServer::listen_accept(void*(*multithreading_func)(void*)){
	int i;
	i=0;
	status = listen(server_socket, 100);
	report_error(status);
	while(1){
		client_socket[i] = accept(server_socket, (struct sockaddr *)&client_sock_addr, &client_addr_len);
		report_error(client_socket[i]);
		status = pthread_create(&tid[i], NULL, multithreading_func, &client_socket[i]);
		report_error(status);
		i++;
	}
}

void TCPServer::read_data(){
	pkt.clear_data();
	status = read(server_socket, pkt.data, BUFFER_SIZE);
	report_error(status);
	pkt.data_len = status;
}

void TCPServer::write_data(){
	status = write(server_socket, pkt.data, pkt.data_len);
	report_error(status);
}

TCPServer::~TCPServer(){
	free(server_addr);
	close(server_socket);
}
