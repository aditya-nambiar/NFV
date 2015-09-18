#include "client.h"

Client::Client() {
	
	client_socket = -1;
	client_addr = allocate_str_mem(INET_ADDRSTRLEN);
	server_addr = allocate_str_mem(INET_ADDRSTRLEN);
	signal(SIGPIPE, SIG_IGN);			
}

Client::Client(const Client &src_obj) {
	
	client_addr = allocate_str_mem(INET_ADDRSTRLEN);
	server_addr = allocate_str_mem(INET_ADDRSTRLEN);
	status = src_obj.status;
	client_socket = src_obj.client_socket;
	client_port = src_obj.client_port;
	strcpy(client_addr, src_obj.client_addr);
	client_sock_addr = src_obj.client_sock_addr;
	pkt = src_obj.pkt;
	server_port = src_obj.server_port;
	strcpy(server_addr, src_obj.server_addr);
	server_sock_addr = src_obj.server_sock_addr;
}

void swap(Client &src_obj, Client &dst_obj) {
	using std::swap;

	swap(src_obj.status, dst_obj.status);
	swap(src_obj.client_socket, dst_obj.client_socket);
	swap(src_obj.client_port, dst_obj.client_port);
	swap(src_obj.client_addr, dst_obj.client_addr);
	swap(src_obj.client_sock_addr, dst_obj.client_sock_addr);
	swap(src_obj.pkt, dst_obj.pkt);
	swap(src_obj.server_port, dst_obj.server_port);
	swap(src_obj.server_addr, dst_obj.server_addr);
	swap(src_obj.server_sock_addr, dst_obj.server_sock_addr);
}

Client& Client::operator=(Client src_obj) {

	swap(*this, src_obj);
	return *this;	
}

Client::Client(Client &&src_obj)
	:Client() {

	swap(*this, src_obj);
}

void Client::bind_client() {
	
	client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	report_error(client_socket, "Error in creating sockets ");	
	status = setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));	
	report_error(status, "At Client side - Error in setting socket options");
	socklen_t len = sizeof(sockaddr_in);
	bzero((char *) &client_sock_addr, sizeof(client_sock_addr));
	client_sock_addr.sin_family = AF_INET;  
	client_sock_addr.sin_addr.s_addr = INADDR_ANY;	
	client_sock_addr.sin_port = 0;
	status = bind(client_socket, (struct sockaddr*)&client_sock_addr, sizeof(client_sock_addr));
	report_error(status, "Error in binding");	
	status = getsockname(client_socket, (struct sockaddr*)&client_sock_addr, &len);
	report_error(status, "At Client side - Error in getting socket name");
	client_port = ntohs(client_sock_addr.sin_port);	
	strcpy(client_addr, inet_ntoa(client_sock_addr.sin_addr));
	//cout << "client binded with port " << client_port << endl;
}

void Client::fill_server_details(int arg_server_port, const char *arg_server_addr) {
	
	server_port = arg_server_port;
	strcpy(server_addr, arg_server_addr);
	bzero((char*)&server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;
	server_sock_addr.sin_port = htons(server_port);
	status = inet_aton(server_addr, &server_sock_addr.sin_addr);
	if (status == 0) {
		cout << "ERROR: In filling server details - Invalid Server IP address " << "***" << server_addr << "***" << endl;
		exit(EXIT_FAILURE);
	}
}

void Client::connect_with_server(int client_num) {
	int new_server_port;
	
	pkt.clear_data();
	pkt.fill_data(0, sizeof(int), client_num);
	pkt.make_data_packet();
	write_data();
	read_data();
	memcpy(&new_server_port, pkt.data, sizeof(int));
	fill_server_details(new_server_port, server_addr);
	// cout << "Client side: Client-" << client_num << " connected with server with port " << new_server_port << endl;	
}

void Client::read_data() {
	
	pkt.clear_data();
	status = recvfrom(client_socket, pkt.data, BUFFER_SIZE, 0, (sockaddr*)&server_sock_addr, &g_addr_len);
	report_error(status);
	pkt.data_len = status;
	//check_conn(status);
}

void Client::write_data() {
	
	status = sendto(client_socket, pkt.packet, pkt.packet_len, 0, (sockaddr*)&server_sock_addr, g_addr_len);
	report_error(status, "Client side: Error in writing data");
}

Client::~Client() {
	
	free(client_addr);
	free(server_addr);
	if (client_socket >= 0)
		close(client_socket);
}
