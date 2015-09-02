#include "server.h"

Server::Server(){
	
	server_socket = -1;
	server_addr = allocate_str_mem(INET_ADDRSTRLEN);
	signal(SIGPIPE, SIG_IGN);	  
}

void Server::begin_thread_pool(int count, void*(*thread_func)(void*)){

	tpool.set_max_threads(count);
	tpool.set_thread_func(thread_func);
	tpool.create_threads();
}

void Server::set_total_connections(int total_connections){

	clients.resize(total_connections);
	tid.resize(total_connections);
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
	
	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	report_error(server_socket);	
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));
	socklen_t len = sizeof(sockaddr_in);
	status = bind(server_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);	
	getsockname(server_socket, (struct sockaddr*)&server_sock_addr, &len);
	server_port = ntohs(server_sock_addr.sin_port);	
	//cout<<"Server binded with port "<<server_port<<endl;
}

void Server::listen_accept(){
	ClientDetails entity;

	while(1){
		status = recvfrom(server_socket, pkt.data, BUFFER_SIZE, 0, (sockaddr*)&entity.client_sock_addr, &g_addr_len);
		report_error(status);
		memcpy(&entity.num, pkt.data, sizeof(int)); 		
		status = pthread_mutex_lock(&tpool.conn_lock);
		report_error(status, "Error in locking");
		if(tpool.connections.size() == tpool.max_threads){
			status = pthread_cond_wait(&tpool.conn_full, &tpool.conn_lock);
			report_error(status, "Error in conditional wait");
		}
		tpool.connections.push(entity);
		status = pthread_cond_signal(&tpool.conn_req);
		report_error(status, "Error in signalling");
		status = pthread_mutex_unlock(&tpool.conn_lock);
		report_error(status, "Error in unlocking");
		// cout<<"Server side: Connection made with Client "<<entity.num<<endl;
	}
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
