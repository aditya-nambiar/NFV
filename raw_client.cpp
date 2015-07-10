#include "raw_client.h"

const int RawClient::on;
const char* RawClient::interface;
struct ifreq RawClient::ifr;

void RawClient::set_interface(const char *interface_inp){
	interface = interface_inp;
	memset(&ifr, 0, sizeof(ifr));
	snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
}

RawClient::RawClient(){
	client_address = allocate_str_mem(INET_ADDRSTRLEN);
	server_address = allocate_str_mem(INET_ADDRSTRLEN);
	client_socket = socket (AF_INET, SOCK_RAW, IPPROTO_UDP);
	report_error(client_socket);
	setsockopt(client_socket, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on));	
	signal(SIGPIPE, SIG_IGN);					
	//status = 0;
	//setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));
}

void RawClient::fill_client_details(int client_port, const char *client_address){
	this->client_port = client_port;
	strcpy(this->client_address, client_address);
	bzero((char*)&client_sock_addr, sizeof(client_sock_addr));
	client_sock_addr.sin_family = AF_INET;
	client_sock_addr.sin_port = htons(client_port);
	status = inet_aton(client_address, &client_sock_addr.sin_addr);
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void RawClient::bind_client(){
	status = ioctl(client_socket, SIOCGIFINDEX, &ifr);
	report_error(status, "IOCTL failed to find interface");
	status = setsockopt(client_socket, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr));
	report_error(status, "Binding to interface failed");
	status = bind(client_socket, (struct sockaddr*)&client_sock_addr, sizeof(client_sock_addr));
	report_error(status);			
}

void RawClient::fill_server_details(int server_port, const char *server_address){
	this->server_port = server_port;
	strcpy(this->server_address, server_address);
	bzero((char*)&server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;
	server_sock_addr.sin_port = htons(server_port);
	status = inet_aton(server_address, &server_sock_addr.sin_addr);
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void RawClient::connect_with_server(Packet &pkt, int client_num){
	int new_server_port;
	path addr;
	addr.src_ip =  client_address;
	addr.dst_ip = server_address;
	addr.src_port = client_port;
	addr.dst_port = server_port;
	pkt.clear_data();
	pkt.fill_data(0, sizeof(int), client_num);
	pkt.make_pkt(addr);
	write_data(pkt);
	read_data(pkt);
	memcpy(&new_server_port, pkt.data, sizeof(int));
	fill_server_details(new_server_port, server_address);
	cout<<"Client side: Client-"<<client_num<<" connected with server"<<endl;	
}

void RawClient::read_data(Packet &pkt){
	pkt.clear_data();
	status = recvfrom(client_socket, pkt.data, IP_MAXPACKET, 0, (sockaddr*)&server_sock_addr, &g_addr_len);
	report_error(status);
	pkt.data_len = strlen((const char*)pkt.data);
	//	check_conn(status);
}

void RawClient::write_data(Packet &pkt){
	status = sendto(client_socket, pkt.packet, pkt.packet_len, 0, (sockaddr*)&server_sock_addr, g_addr_len);
	report_error(status);
}

RawClient::~RawClient(){
	free(client_address);
	free(server_address);
	close(client_socket);
}