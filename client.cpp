#include "client.h"

Client::Client(){
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	report_error(client_socket);
}

void Client::input_server_details(int server_port, const char *server_address){
	int status;
	this->server_port = server_port;
	this->server_address = server_address;
	bzero((char*)&server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;
	server_sock_addr.sin_port = htons(server_port);
	status = inet_aton(server_address, &server_sock_addr.sin_addr);
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void Client::connect_with_server(){
	int status;
	signal(SIGPIPE, SIG_IGN);
	status = connect(client_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);
	print_message("Connected with Server");
}

void Client::read_data(){
	int status;
	bzero(client_buffer, BUFFER_SIZE);
	status = read(client_socket, client_buffer, BUFFER_SIZE-1);
	report_error(status);
}

void Client::write_data(){
	int status;
	status = write(client_socket, client_buffer, BUFFER_SIZE-1);
	report_error(status);
}

Client::~Client(){
	close(client_socket);
}
