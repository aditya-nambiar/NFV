#include "server.h"

Server::Server() {
	
	server_socket = -1;
	server_addr = allocate_str_mem(INET_ADDRSTRLEN);
	signal(SIGPIPE, SIG_IGN);	  
}

Server::Server(const Server &src_obj) {

	server_addr = allocate_str_mem(INET_ADDRSTRLEN);
	status = src_obj.status;
	server_port = src_obj.server_port;
	strcpy(server_addr, src_obj.server_addr);
	server_socket = src_obj.server_socket;
	server_sock_addr = src_obj.server_sock_addr;
	Packet pkt = src_obj.pkt;
	client_num = src_obj.client_num;
	client_sock_addr = src_obj.client_sock_addr;
	clients = src_obj.clients;
	tid = src_obj.tid;	
	tpool = src_obj.tpool;
}

void swap(Server &src_obj, Server &dst_obj) {
	using std::swap;

	swap(src_obj.status, dst_obj.status);
	swap(src_obj.server_port, dst_obj.server_port);
	swap(src_obj.server_addr, dst_obj.server_addr);
	swap(src_obj.server_socket, dst_obj.server_socket);
	swap(src_obj.server_sock_addr, dst_obj.server_sock_addr);
	swap(src_obj.pkt, dst_obj.pkt);
	swap(src_obj.client_num, dst_obj.client_num);
	swap(src_obj.client_sock_addr, dst_obj.client_sock_addr);
	swap(src_obj.clients, dst_obj.clients);
	swap(src_obj.tid, dst_obj.tid);
	swap(src_obj.tpool, dst_obj.tpool);
}

Server& Server::operator=(Server src_obj) {

	swap(*this, src_obj);
	return *this;	
}

Server::Server(Server&& src_obj)
	:Server() {

	swap(*this, src_obj);
}

void Server::begin_thread_pool(int count, void*(*thread_func)(void*)) {

	tpool.set_max_threads(count);
	tpool.set_thread_func(thread_func);
	tpool.create_threads();
}

void Server::set_total_connections(int total_connections) {

	clients.resize(total_connections);
	tid.resize(total_connections);
}

void Server::fill_server_details(int arg_server_port, const char *arg_server_addr) {
	
	strcpy(server_addr, arg_server_addr);
	bzero((char *) &server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;  	
	server_sock_addr.sin_port = (arg_server_port)?htons(arg_server_port):arg_server_port;
	status = inet_aton(server_addr, &server_sock_addr.sin_addr);	
	if (status == 0) {
		cout << "ERROR: Invalid IP address" << endl;
		exit(EXIT_FAILURE);
	}
}

void Server::bind_server() {
	
	server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	report_error(server_socket);	
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));
	socklen_t len = sizeof(sockaddr_in);
	status = bind(server_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);	
	getsockname(server_socket, (struct sockaddr*)&server_sock_addr, &len);
	server_port = ntohs(server_sock_addr.sin_port);	
	//cout << "Server binded with port " << server_port << endl;
}

void Server::listen_accept() {
	ClientDetails entity;

	while (1) {
		status = recvfrom(server_socket, pkt.data, BUFFER_SIZE, 0, (sockaddr*)&entity.client_sock_addr, &g_addr_len);
		report_error(status);
		memcpy(&entity.num, pkt.data, sizeof(int)); 		
		status = pthread_mutex_lock(&tpool.conn_lock);
		report_error(status, "Error in locking");
		if (tpool.connections.size() == tpool.max_threads) {
			status = pthread_cond_wait(&tpool.conn_full, &tpool.conn_lock);
			report_error(status, "Error in conditional wait");
		}
		tpool.connections.push(entity);
		status = pthread_cond_signal(&tpool.conn_req);
		report_error(status, "Error in signalling");
		status = pthread_mutex_unlock(&tpool.conn_lock);
		report_error(status, "Error in unlocking");
		// cout << "Server side: Connection made with Client " << entity.num << endl;
	}
}

void Server::listen_accept(void*(*multithreading_func)(void*)) {
	int i;
	
	i = 0;
	while (1) {
		status = recvfrom(server_socket, pkt.data, BUFFER_SIZE, 0, (sockaddr*)&clients[i].client_sock_addr, &g_addr_len);
		memcpy(&clients[i].num, pkt.data, sizeof(int)); 		
		report_error(status);
		status = pthread_create(&tid[i], NULL, multithreading_func, &clients[i]);
		report_error(status);
		cout << "Server side: Connection made with Client " << clients[i].num << endl;
		i++;
	}
}

void Server::connect_with_client() {
	
	pkt.clear_data();
	pkt.fill_data(0, sizeof(int), server_port);
	pkt.make_data_packet();
	write_data();
}

void Server::read_data() {
	
	pkt.clear_data();
	status = recvfrom(server_socket, pkt.data, BUFFER_SIZE, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
	report_error(status);
	pkt.data_len = status;
	//check_conn(status);
}

void Server::write_data() {
	
	status = sendto(server_socket, pkt.packet, pkt.packet_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);
}

Server::~Server() {
	
	free(server_addr);
	if (server_socket >= 0)
		close(server_socket);
}
