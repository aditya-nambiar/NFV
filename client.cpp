#include "client.h"

Client::Client(){
	client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	report_error(client_socket);
	client_addr = allocate_str_mem(INET_ADDRSTRLEN);
	server_addr = allocate_str_mem(INET_ADDRSTRLEN);
	signal(SIGPIPE, SIG_IGN);			
	//status = 0;
	//setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));
}

void Client::bind_client(){
	socklen_t len = sizeof(sockaddr_in);
	bzero((char *) &client_sock_addr, sizeof(client_sock_addr));
	client_sock_addr.sin_family = AF_INET;  
	client_sock_addr.sin_addr.s_addr = INADDR_ANY;	
	client_sock_addr.sin_port = 0;
	status = bind(client_socket, (struct sockaddr*)&client_sock_addr, sizeof(client_sock_addr));
	report_error(status);	
	getsockname(client_socket, (struct sockaddr*)&client_sock_addr, &len);
	client_port = ntohs(client_sock_addr.sin_port);	
	strcpy(client_addr, inet_ntoa(client_sock_addr.sin_addr));
	//cout<<"client binded with port "<<client_port<<endl;
}

void Client::fill_server_details(int server_port, const char *server_addr){
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

void Client::connect_with_server(int client_num){
	int new_server_port;
	pkt.clear_data();
	pkt.fill_data(0, sizeof(int), client_num);
	pkt.make_data_packet();
	write_data();
	read_data();
	memcpy(&new_server_port, pkt.data, sizeof(int));
	fill_server_details(new_server_port, server_addr);
	cout<<"Client side: Client-"<<client_num<<" connected with server with port "<<new_server_port<<endl;	
}

void Client::read_data(){
	pkt.clear_data();
	status = recvfrom(client_socket, pkt.data, BUFFER_SIZE, 0, (sockaddr*)&server_sock_addr, &g_addr_len);
	report_error(status);
	pkt.data_len = status;
	//check_conn(status);
}

void Client::write_data(){
	status = sendto(client_socket, pkt.packet, pkt.packet_len, 0, (sockaddr*)&server_sock_addr, g_addr_len);
	report_error(status);
}

Client::~Client(){
	free(client_addr);
	free(server_addr);
	close(client_socket);
}
