#include "pgwu.h"

unordered_map<string, TunUdata> g_tun_utable;

TunUdata::TunUdata(){

	// Dummy
};

TunUdata::~TunUdata(){

	// Dummy
}

PGWu::PGWu(){

	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
}

uint16_t PGWu::generate_uteid(int &ue_num){

	return ue_num;
}

void PGWu::configure_raw_client(){

	RawSocket::set_interface("tun1");
	raw_client.bind_client();	
	raw_client.fill_dst_details();
	cout<<"Raw client configured successfully"<<endl;
}

void PGWu::configure_server_for_sink(){

	for_sink.fill_server_details(g_pgw_server_for_sink_port, g_pgw_server_for_sink_addr);
	for_sink.bind_server();
	cout<<"Server for sink configured successfully"<<endl;
}

void PGWu::recv_sgw(Server &pgw_server){

	pgw_server.read_data();
	pgw_server.pkt.rem_gtpu_hdr();
	copy_to_rawpkt(pgw_server.pkt);
	//cout<<endl<<"Received data from SGW and removed GTPu header successfully"<<endl;
}

void PGWu::copy_to_rawpkt(Packet &arg){

	raw_client.pkt.clear_data();
	raw_client.pkt.fill_data(0, arg.data_len, arg.data);
}

void PGWu::send_sgw(Server &pgw_server){
	struct ip *iphdr = (ip*)malloc(20 * sizeof(u_int8_t));
	struct tcphdr *tcp_hdr = (tcphdr*)malloc(20 * sizeof(u_int8_t)); 
	char *sink = (char*)malloc(INET_ADDRSTRLEN);

	memcpy(iphdr, for_sink.pkt.data, 20 * sizeof(uint8_t));
	memcpy(tcp_hdr, for_sink.pkt.data + 20 * sizeof(uint8_t), 20 * sizeof(uint8_t));	
	inet_ntop(AF_INET, &(iphdr->ip_dst), sink, INET_ADDRSTRLEN);
	//cout<<"At PGW: UE IP is "<<sink<<endl;
	//cout<<"TCP destination port is "<<ntohs(tcp_hdr->th_dport)<<endl;	

	copy_sinkpkt_to_pgwpkt(pgw_server.pkt);
	pgw_server.pkt.fill_gtpu_hdr(tun_udata.sgw_uteid);
	pgw_server.pkt.add_gtpu_hdr();
	pgw_server.pkt.make_data_packet();
	pgw_server.write_data();
	//cout<<"Sent data to SGW successfully"<<endl<<endl;
}

void PGWu::copy_sinkpkt_to_pgwpkt(Packet &arg){

	arg.clear_data();
	arg.fill_data(0, for_sink.pkt.data_len, for_sink.pkt.data);
}

void PGWu::send_raw_socket(){

	raw_client.pkt.make_data_packet();
	raw_client.write_data();
	//cout<<"Sent data through raw socket successfully"<<endl<<endl;
}

void PGWu::recv_sink(){

	for_sink.read_data();
	//cout<<endl<<"Data received from sink successfully of "<<for_sink.pkt.data_len<<" bytes"<<endl;
}

void PGWu::set_ue_ip(){
	struct ip *iphdr = (ip*)malloc(20 * sizeof(u_int8_t));

	memcpy(iphdr, for_sink.pkt.data, 20 * sizeof(uint8_t));
	inet_ntop(AF_INET, &(iphdr->ip_dst), ue_ip, INET_ADDRSTRLEN);
}

void PGWu::set_tun_udata(){
	string ue_ip_str;

	ue_ip_str.assign(ue_ip);
	tun_udata = g_tun_utable[ue_ip_str];
}

void PGWu::fill_tun_utable(string ue_ip, TunUdata &tun_udata){

	g_tun_utable[ue_ip] = tun_udata;
}

PGWu::~PGWu(){

	free(ue_ip);
}
