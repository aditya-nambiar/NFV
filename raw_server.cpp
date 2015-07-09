#include "raw_server.h"

int g_reuse = 1;

const char* RawServer::interface;
struct ifreq RawServer::ifr;

void RawServer::set_interface(const char *interface){
	this->interface = interface;
	memset(&ifr, 0, sizeof(ifr));
	snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", this->interface);
}

RawServer::RawServer(){
	server_address = allocate_str_mem(INET_ADDRSTRLEN);
	server_socket = socket (AF_INET, SOCK_RAW, IPPROTO_UDP);
	report_error(server_socket);
	setsockopt(server_socket, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on));
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));
	signal(SIGPIPE, SIG_IGN);	  
}

void RawServer::fill_server_details(int server_port, const char *server_address){
	this->server_port = server_port;
	strcpy(this->server_address, server_address);
	bzero((char *) &server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;  	
	server_sock_addr.sin_port = htons(server_port); 
	status = inet_aton(server_address, &server_sock_addr.sin_addr);
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void RawServer::bind_server(){
	status = ioctl(server_socket, SIOCGIFINDEX, &ifr);
	report_error(status, "IOCTL failed to find interface");
	status = setsockopt(server_socket, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr));
	report_error(status, "Binding to interface failed")
	status = bind(server_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);			
}

void RawServer::listen_accept(void*(*multithreading_func)(void*), Packet pkt){
	int i;
	i=0;
	while(1){
		pkt.clear_data();
		status = recvfrom(server_socket, pkt.data, IP_MAXPACKET, 0, (sockaddr*)&clients[i].client_sock_addr, &g_addr_len);
		report_error(status);
		pkt.data_len = strlen(pkt.data);
		memcpy(&clients[i].num, pkt.data, sizeof(int)); 		
		status = pthread_create(&tid[i], NULL, multithreading_func, &clients[i]);
		report_error(status);
		cout<<"RawServer side: Connection made with Client"<<clients[i].num<<endl;
		i++;
	}
}

void RawServer::connect_with_client(Packet pkt){
	link addr;
	addr.src_ip = server_address;
	addr.dst_ip = inet_ntoa(client_sock_addr.sin_addr);
	addr.src_port = server_port;
	addr.dst_port = ntohs(client_sock_addr.sin_port);
	pkt.clear_data();
	pkt.fill_data(0, sizeof(int), server_port);
	pkt.make_pkt(addr);
	write_data(pkt);
}

void RawServer::read_data(Packet &pkt){
	pkt.clear_data();
	status = recvfrom(server_socket, pkt.data, IP_MAXPACKET, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
	report_error(status);
	pkt.data_len = strlen(pkt.data);
	//	check_conn(status);
}

void RawServer::write_data(char *packet, int packet_len){
	status = sendto(server_socket, pkt.packet, pkt.packet_len, 0, (sockaddr*)&client_sock_addr, g_addr_len);
	report_error(status);
}

RawServer::~RawServer(){
	free(server_address);	
	close(server_socket);
}
