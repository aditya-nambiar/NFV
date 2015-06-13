#include "client.h"

CLIENT::CLIENT(){
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	report_error(client_socket);
}

void CLIENT::input_server_details(int server_port, const char *server_address){
	int status;
	this->server_port = server_port;
	this->server_address = server_address;
	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	status = inet_aton(server_address, &server_addr.sin_addr);
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void CLIENT::connect_with_server(){
	int status;
	signal(SIGPIPE, SIG_IGN);
	status = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	report_error(status);
	print_message("Connected with Server");
}

CLIENT::~CLIENT(){
	close(client_socket);
}
