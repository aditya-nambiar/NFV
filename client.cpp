#include "client.h"

Client::Client(){
	client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	report_error(client_socket);
	status = 0;
	//setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));
	signal(SIGPIPE, SIG_IGN);			
}

void Client::fill_server_details(int server_port, const char *server_address){
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

void Client::connect_with_server(int client_num){
	int new_server_port;
	char new_server_address[BUFFER_SIZE];
	memcpy(client_buffer, &client_num, sizeof(int));
	write_data();
	read_data();
	memcpy(&new_server_port, client_buffer, sizeof(int));
	memcpy(new_server_address, client_buffer+sizeof(int), sizeof(client_buffer)-sizeof(int));
	fill_server_details(new_server_port, new_server_address);
	cout<<"Client side: Client-"<<client_num<<" connected with server"<<endl;
	
}

void Client::read_data(){
	bzero(client_buffer, BUFFER_SIZE);
	status = recvfrom(client_socket, client_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&server_sock_addr, &g_addr_len);
	check_conn(status);
	report_error(status);
}

void Client::write_data(){
	status = sendto(client_socket, client_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&server_sock_addr, g_addr_len);
	report_error(status);
}

Client::~Client(){
	close(client_socket);
}
	/*long long buf_size;
	socklen_t s_opt = sizeof(long long);
	status =  getsockopt(client_socket, SOL_SOCKET, SO_SNDBUF, &buf_size, &s_opt);		
	report_error(status);
	cout<<"Size is "<<g_stack_buf<<" "<<buf_size<<endl;*/
