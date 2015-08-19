#include "tcp_client.h"

TCPClient::TCPClient(){
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	report_error(client_socket);
	client_addr = allocate_str_mem(INET_ADDRSTRLEN);
	server_addr = allocate_str_mem(INET_ADDRSTRLEN);
	signal(SIGPIPE, SIG_IGN);			
}

void TCPClient::fill_client_details(const char* client_addr){
	strcpy(this->client_addr, client_addr);
	bzero((char *) &client_sock_addr, sizeof(client_sock_addr));
	client_sock_addr.sin_family = AF_INET;  	
	client_sock_addr.sin_port = 0;
	status = inet_aton(client_addr, &client_sock_addr.sin_addr);	
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void TCPClient::bind_client(){
	socklen_t len = sizeof(sockaddr_in);
	status = bind(client_socket, (struct sockaddr*)&client_sock_addr, sizeof(client_sock_addr));
	report_error(status);	
	getsockname(client_socket, (struct sockaddr*)&client_sock_addr, &len);
	client_port = ntohs(client_sock_addr.sin_port);	
}

void TCPClient::fill_server_details(int server_port, const char *server_addr){
	this->server_port = server_port;
	strcpy(this->server_addr, server_addr);
	bzero((char*)&server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;
	server_sock_addr.sin_port = htons(server_port);
	status = inet_aton(server_addr, &server_sock_addr.sin_addr);
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void TCPClient::connect_with_server(){
	status = connect(client_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);
	cout<<"Successfully connected with server "<<endl;
}

void TCPClient::read_data(){
	pkt.clear_data();
	status = read(client_socket, pkt.data, BUFFER_SIZE);
	report_error(status);
	pkt.data_len = status;
}

void TCPClient::write_data(){
	status = write(client_socket, pkt.packet, pkt.packet_len);
	report_error(status);
}

TCPClient::~TCPClient(){
	free(client_addr);
	free(server_addr);
	close(client_socket);
}
