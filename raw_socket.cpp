#include "raw_socket.h"

const int RawSocket::on;
const char* RawSocket::interface;
struct ifreq RawSocket::ifr;

void RawSocket::set_interface(const char *interface_inp){
	interface = interface_inp;
	memset(&ifr, 0, sizeof(ifr));
	snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
	cout<<"hey its here 1"<<endl;
}

RawSocket::RawSocket(){
	src_addr = allocate_str_mem(INET_ADDRSTRLEN);
	dst_addr = allocate_str_mem(INET_ADDRSTRLEN);
	raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	report_error(raw_socket);
	setsockopt(raw_socket, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on));	
	signal(SIGPIPE, SIG_IGN);					
	cout<<"hey its here 2"<<endl;
	//status = 0;
	//setsockopt(raw_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));
}

void RawSocket::bind_client(){
	status = ioctl(raw_socket, SIOCGIFINDEX, &ifr);
	report_error(status, "IOCTL failed to find interface");
	status = setsockopt(raw_socket, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr));
	report_error(status, "Binding to interface failed");
	cout<<"hey its here 3"<<endl;
}

void RawSocket::fill_traffic_details(Packet &arg_pkt){
	cout<<"This is the end 1"<<endl;
	memcpy(&pkt.ip_hdr, arg_pkt.data, IP_LEN * sizeof(uint8_t));
	cout<<"This is the end 2"<<endl;
	memcpy(&pkt.udp_hdr, arg_pkt.data + IP_LEN * sizeof(uint8_t), UDP_LEN * sizeof(uint8_t));
	cout<<"This is the end 3"<<endl;
	pkt.data_len = arg_pkt.data_len - IP_LEN * sizeof(uint8_t) - UDP_LEN * sizeof(uint8_t);
	cout<<"This is the end 4"<<endl;
	memcpy(pkt.data, arg_pkt.data + IP_LEN * sizeof(uint8_t) + UDP_LEN * sizeof(uint8_t), pkt.data_len);
	cout<<"This is the end 5"<<endl;
	pkt.packet_len = arg_pkt.data_len;
	memcpy(pkt.packet, arg_pkt.data, pkt.packet_len);
	cout<<"This is the end 6"<<endl;
	fill_src_details();
	fill_dst_details();
	cout<<"hey its here 4"<<endl;
}

void RawSocket::fill_src_details(){
	src_port = ntohs(pkt.udp_hdr.source);		
	if(!inet_ntop(AF_INET, &(pkt.ip_hdr.ip_src), src_addr, INET_ADDRSTRLEN))
		report_error(status, "this is the error");	
	
}

void RawSocket::fill_dst_details(){
	dst_port = ntohs(pkt.udp_hdr.dest);
	inet_ntop(AF_INET, &(pkt.ip_hdr.ip_dst), dst_addr, INET_ADDRSTRLEN);
	bzero((char*)&dst_sock_addr, sizeof(dst_sock_addr));
	dst_sock_addr.sin_family = AF_INET;
	dst_sock_addr.sin_port = htons(dst_port);
	status = inet_aton(dst_addr, &dst_sock_addr.sin_addr);
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
}

void RawSocket::write_data(){
	cout<<"hey its here 5"<<endl;
	status = sendto(raw_socket, pkt.packet, pkt.packet_len, 0, (sockaddr*)&dst_sock_addr, g_addr_len);
	report_error(status);
}

RawSocket::~RawSocket(){
	free(src_addr);
	free(dst_addr);
	close(raw_socket);
}