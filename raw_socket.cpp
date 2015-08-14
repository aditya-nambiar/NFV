#include "raw_socket.h"

const int RawSocket::on;
const char* RawSocket::interface;
struct ifreq RawSocket::ifr;

void RawSocket::set_interface(const char *interface_inp){
	interface = interface_inp;
	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
}

RawSocket::RawSocket(){
	// src_addr = allocate_str_mem(INET_ADDRSTRLEN);
	// dst_addr = allocate_str_mem(INET_ADDRSTRLEN);
	raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	report_error(raw_socket);
	setsockopt(raw_socket, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));	
	signal(SIGPIPE, SIG_IGN);					
	//status = 0;
	//setsockopt(raw_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));
}

void RawSocket::bind_client(){
	status = ioctl(raw_socket, SIOCGIFINDEX, &ifr);
	report_error(status, "IOCTL failed to find interface");
	status = setsockopt(raw_socket, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr));
	report_error(status, "Binding to interface failed");
}

// void RawSocket::fill_traffic_details(Packet &arg_pkt){
// 	memcpy(pkt.packet, arg_pkt.data, arg_pkt.data_len * sizeof(uint8_t));
// 	pkt.packet_len = arg_pkt.data_len;
// 	memcpy(&pkt.ip_hdr, pkt.packet, IP_LEN * sizeof(uint8_t));
// 	memcpy(&pkt.udp_hdr, pkt.packet + IP_LEN * sizeof(uint8_t), UDP_LEN * sizeof(uint8_t));
// 	fill_src_details();
// 	fill_dst_details();
// }

// void RawSocket::fill_src_details(){
// 	src_port = ntohs(pkt.udp_hdr.source);
// 	if(!inet_ntop(AF_INET, &(pkt.ip_hdr.ip_src), src_addr, INET_ADDRSTRLEN))
// 		report_error(status, "this is the error");	
// 	cout<<"Source port is "<<src_port<<endl;
// 	cout<<"Source address is "<<src_addr<<endl;	
// }

// void RawSocket::fill_dst_details(){
// 	dst_port = ntohs(pkt.udp_hdr.dest);
// 	inet_ntop(AF_INET, &(pkt.ip_hdr.ip_dst), dst_addr, INET_ADDRSTRLEN);
// 	bzero((char*)&dst_sock_addr, sizeof(dst_sock_addr));
// 	dst_sock_addr.sin_family = AF_INET;
// 	dst_sock_addr.sin_port = htons(dst_port);
// 	status = inet_aton(dst_addr, &dst_sock_addr.sin_addr);
// 	if(status == 0){
// 		cout<<"ERROR: Invalid IP address"<<endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	cout<<"Destination port is "<<dst_port<<endl;
// 	cout<<"Destination address is "<<dst_addr<<endl;	
// }

void RawSocket::write_data(){
	status = sendto(raw_socket, pkt.packet, pkt.packet_len, 0, (sockaddr*)&dst_sock_addr, g_addr_len);
	report_error(status);
}

RawSocket::~RawSocket(){
	// free(src_addr);
	// free(dst_addr);
	close(raw_socket);
}