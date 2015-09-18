#include "pgwu.h"

unordered_map<string, TunUdata> g_tun_utable;

TunUdata::TunUdata() {

	// Dummy
};

TunUdata::TunUdata(const TunUdata &src_obj) {

	sgw_uteid = src_obj.sgw_uteid;
}

void swap(TunUdata &src_obj, TunUdata &dst_obj) {
	using std::swap;

	swap(src_obj.sgw_uteid, dst_obj.sgw_uteid);	
}

TunUdata& TunUdata::operator=(TunUdata src_obj) {

	swap(*this, src_obj);
	return *this;
}

TunUdata::TunUdata(TunUdata &&src_obj)
	:TunUdata() {

	swap(*this, src_obj);
}

TunUdata::~TunUdata() {

	// Dummy
}

PGWu::PGWu() {

	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
}

PGWu::PGWu(const PGWu &src_obj) {
	
	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	raw_client = src_obj.raw_client;
	for_sink = src_obj.for_sink;
	strcpy(ue_ip, src_obj.ue_ip);
	tun_udata = src_obj.tun_udata;
}

void swap(PGWu &src_obj, PGWu &dst_obj) {
	using std::swap;

	swap(src_obj.raw_client, dst_obj.raw_client);
	swap(src_obj.for_sink, dst_obj.for_sink);
	swap(src_obj.ue_ip, dst_obj.ue_ip);
	swap(src_obj.tun_udata, dst_obj.tun_udata);
}

PGWu& PGWu::operator=(PGWu src_obj) {

	swap(*this, src_obj);
	return *this;
}

PGWu::PGWu(PGWu &&src_obj)
	:PGWu() {

	swap(*this, src_obj);
}

uint16_t PGWu::generate_uteid(int &ue_number) {

	return ue_number;
}

void PGWu::configure_raw_client() {

	RawSocket::set_interface("tun1");
	raw_client.bind_client();	
	cout << "Raw client configured successfully" << endl;
}

void PGWu::configure_server_for_sink() {

	for_sink.fill_server_details(g_pgw_server_for_sink_port, g_pgw_server_for_sink_addr);
	for_sink.bind_server();
	cout << "Server for sink configured successfully" << endl;
}

void PGWu::recv_sgw(Server &pgw_server) {

	pgw_server.read_data();
	pgw_server.pkt.rem_gtpu_hdr();
	copy_to_rawpkt(pgw_server.pkt);
	//cout << endl << "Received data from SGW and removed GTPu header successfully" << endl;
}

void PGWu::copy_to_rawpkt(Packet &arg) {

	raw_client.pkt.clear_data();
	raw_client.pkt.fill_data(0, arg.data_len, arg.data);
}

void PGWu::send_sgw(Server &pgw_server) {
	struct ip *iphdr = allocate_ip_mem(20);
	struct tcphdr *tcp_hdr = allocate_tcp_mem(20);
	char *sink = allocate_str_mem(INET_ADDRSTRLEN);

	memcpy(iphdr, for_sink.pkt.data, 20 * sizeof(uint8_t));
	memcpy(tcp_hdr, for_sink.pkt.data + 20 * sizeof(uint8_t), 20 * sizeof(uint8_t));	
	inet_ntop(AF_INET, &(iphdr->ip_dst), sink, INET_ADDRSTRLEN);
	//cout << "At PGW: UE IP is " << sink << endl;
	//cout << "TCP destination port is " << ntohs(tcp_hdr->th_dport) << endl;	

	copy_sinkpkt_to_pgwpkt(pgw_server.pkt);
	pgw_server.pkt.fill_gtpu_hdr(tun_udata.sgw_uteid);
	pgw_server.pkt.add_gtpu_hdr();
	pgw_server.pkt.make_data_packet();
	pgw_server.write_data();
	//cout << "Sent data to SGW successfully" << endl << endl;
	free(iphdr);
	free(tcp_hdr);
	free(sink);
}

void PGWu::copy_sinkpkt_to_pgwpkt(Packet &arg) {

	arg.clear_data();
	arg.fill_data(0, for_sink.pkt.data_len, for_sink.pkt.data);
}

void PGWu::send_raw_socket() {

	raw_client.fill_dst_details();
	raw_client.pkt.make_data_packet();
	raw_client.write_data();
	//cout << "Sent data through raw socket successfully" << endl << endl;
}

void PGWu::recv_sink() {

	for_sink.read_data();
	//cout << endl << "Data received from sink successfully of " << for_sink.pkt.data_len << " bytes" << endl;
}

void PGWu::set_ue_ip() {
	struct ip *iphdr = allocate_ip_mem(20);

	memcpy(iphdr, for_sink.pkt.data, 20 * sizeof(uint8_t));
	inet_ntop(AF_INET, &(iphdr->ip_dst), ue_ip, INET_ADDRSTRLEN);
	free(iphdr);
}

void PGWu::set_tun_udata(bool &data_invalid) {
	string ue_ip_str;

	ue_ip_str.assign(ue_ip);
	if (g_tun_utable.find(ue_ip_str) != g_tun_utable.end()) {
		tun_udata = g_tun_utable[ue_ip_str];
		data_invalid = false;
	}
	else {
		data_invalid = true;
	}
}

void PGWu::fill_tun_utable(string ue_ip, TunUdata &tun_udata) {

	g_tun_utable[ue_ip] = tun_udata;
}

void PGWu::erase_tun_utable(string ue_ip) {

	// g_tun_utable.erase(ue_ip); /* This is to make the leftover data traffic to reach its intended destination  */
}

PGWu::~PGWu() {

	free(ue_ip);
}
