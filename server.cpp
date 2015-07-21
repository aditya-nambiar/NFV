#include "server.h"

int g_reuse = 1;

Server::Server(){
	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	report_error(server_socket);
	server_addr = allocate_str_mem(INET_ADDRSTRLEN);
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));
	signal(SIGPIPE, SIG_IGN);	  
}

void Server::fill_server_details(int server_port, const char *server_addr){
	strcpy(this->server_addr, server_addr);
	bzero((char *) &server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;  	
	server_sock_addr.sin_port = (server_port)?htons(server_port):server_port;
	status = inet_aton(server_addr, &server_sock_addr.sin_addr);	
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void Server::bind_server(){
	socklen_t len = sizeof(sockaddr_in);
	status = bind(server_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);	
	getsockname(server_socket, (struct sockaddr*)&server_sock_addr, &len);
	server_port = ntohs(server_sock_addr.sin_port);	
	//cout<<"Server binded with port "<<server_port<<endl;
}

void Server::listen_accept(void*(*multithreading_func)(void*)){
	int i;
	i=0;
	while(1){
		status = recvfrom(server_socket, pkt.data, BUFFER_SIZE, 0, (sockaddr*)&clients[i].client_sock_addr, &g_addr_len);
		memcpy(&clients[i].num, pkt.data, sizeof(int)); 		
		report_error(status);
		status = pthread_create(&tid[i], NULL, multithreading_func, &clients[i]);
		report_error(status);
		cout<<"Server side: Connection made with Client "<<clients[i].num<<endl;
		i++;
	}
}

void Server::connect_with_client(){
	pkt.clear_data();
	pkt.fill_data(0, sizeof(int), server_port);
	pkt.make_data_packet();
	write_data();
}

void Server::read_data(){
	pkt.clear_data();
	status = recvfrom(server_socket, pkt.data, BUFFER_SIZE, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
	report_error(status);
	pkt.data_len = status;
	//check_conn(status);
}

void Server::write_data(){
	status = sendto(server_socket, pkt.packet, pkt.packet_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);
}

Server::~Server(){
	free(server_addr);
	close(server_socket);
}
