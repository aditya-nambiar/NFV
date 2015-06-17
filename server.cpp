#include "server.h"

Server::Server(int server_port, const char *server_address){
	this->server_port = server_port;
	this->server_address = server_address;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	report_error(server_socket);
  bzero((char *) &server_sock_addr, sizeof(server_sock_addr));
  server_sock_addr.sin_family = AF_INET;
  server_sock_addr.sin_port = htons(server_port); 
  inet_aton(server_address, &server_sock_addr.sin_addr);
  client_addr_len = sizeof(client_sock_addr);	
}

void Server::start_listening(){
	int status;
	status = bind(server_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);
	signal(SIGPIPE, SIG_IGN);
  status = listen(server_socket, 10);
  report_error(status);
}

void Server::accept_connections(void*(*multithreading_func)(void*)){
	int i=0, status;
	while(1){
		client_socket[i] = accept(server_socket, (struct sockaddr *)&client_sock_addr, &client_addr_len);
		report_error(client_socket[i]);
		print_message("Connected with Client");
		status = pthread_create(&tid[i],NULL,multithreading_func,&client_socket[i]);
		report_error(status);
		i++;
	}
}

void Server::read_data(){
	int status;
	bzero(server_buffer, BUFFER_SIZE);
	status = read(server_socket, server_buffer, BUFFER_SIZE-1);
	report_error(status);
}

void Server::write_data(){
	int status;
	status = write(server_socket, server_buffer, BUFFER_SIZE-1);
	report_error(status);
}

Server::~Server(){
	close(server_socket);
}
