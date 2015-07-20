#include "raw_socket.h"

const int RawSocket::on;
const char* RawSocket::interface;
struct ifreq RawSocket::ifr;

void RawSocket::set_interface(const char *interface_inp){
	interface = interface_inp;
	memset(&ifr, 0, sizeof(ifr));
	snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
}

RawSocket::RawSocket(){
	src_addr = allocate_str_mem(INET_ADDRSTRLEN);
	dst_addr = allocate_str_mem(INET_ADDRSTRLEN);
	raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	report_error(raw_socket);
	setsockopt(raw_socket, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on));	
	signal(SIGPIPE, SIG_IGN);					
	//status = 0;
	//setsockopt(raw_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));
}

void RawSocket::src_details(int src_port, const char *src_addr){
	this->src_port = src_port;
	strcpy(this->src_addr, src_addr);
}

void RawSocket::dst_details(int dst_port, const char *dst_addr){
	this->dst_port = dst_port;
	strcpy(this->dst_addr, dst_addr);
	bzero((char*)&dst_sock_addr, sizeof(dst_sock_addr));
	dst_sock_addr.sin_family = AF_INET;
	dst_sock_addr.sin_port = htons(dst_port);
	status = inet_aton(dst_addr, &dst_sock_addr.sin_addr);
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void RawSocket::bind_client(){
	status = ioctl(raw_socket, SIOCGIFINDEX, &ifr);
	report_error(status, "IOCTL failed to find interface");
	status = setsockopt(raw_socket, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr));
	report_error(status, "Binding to interface failed");
	// status = bind(client_raw_socket, (struct sockaddr*)&client_sock_addr, sizeof(client_sock_addr));
	// report_error(status);			
}

void RawSocket::write_data(Packet &pkt){
	pkt.fill_udp_hdr(src_port, dst_port);
	pkt.fill_ip_hdr(src_addr, dst_addr);
	pkt.encap();
	status = sendto(raw_socket, pkt.packet, pkt.packet_len, 0, (sockaddr*)&dst_sock_addr, g_addr_len);
	report_error(status);
}

RawSocket::~RawSocket(){
	free(src_addr);
	free(dst_addr);
	close(raw_socket);
}