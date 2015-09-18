#include "sgwu.h"

unordered_map<uint16_t, TunUdata> g_tun_utable;

TunUdata::TunUdata() {
	

}

TunUdata::TunUdata(const TunUdata &src_obj) {

	enodeb_uteid = src_obj.enodeb_uteid;
	pgw_uteid = src_obj.pgw_uteid;
	pgw_port = src_obj.pgw_port;
	pgw_addr = src_obj.pgw_addr;
}

void swap(TunUdata &src_obj, TunUdata &dst_obj) {
	using std::swap;

	swap(src_obj.enodeb_uteid, dst_obj.enodeb_uteid);
	swap(src_obj.pgw_uteid, dst_obj.pgw_uteid);
	swap(src_obj.pgw_port, dst_obj.pgw_port);
	swap(src_obj.pgw_addr, dst_obj.pgw_addr);
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
	

}

SGWu::SGWu() {
	
	socket_table.clear();
	to_pgw.resize(UDP_LINKS);
	pos = 0;
	type = 2;
}

SGWu::SGWu(const SGWu &src_obj) {

	socket_table = src_obj.socket_table; 
	to_pgw = src_obj.to_pgw; 
	pkt = src_obj.pkt; 
	uteid = src_obj.uteid; 
	pos = src_obj.pos; 
	num = src_obj.num; 
	type = src_obj.type; 
	tun_udata = src_obj.tun_udata; 
}

void swap(SGWu &src_obj, SGWu &dst_obj) {
	using std::swap;

	swap(src_obj.socket_table, dst_obj.socket_table); 
	swap(src_obj.to_pgw, dst_obj.to_pgw); 
	swap(src_obj.pkt, dst_obj.pkt); 
	swap(src_obj.uteid, dst_obj.uteid); 
	swap(src_obj.pos, dst_obj.pos); 
	swap(src_obj.num, dst_obj.num); 
	swap(src_obj.type, dst_obj.type); 
	swap(src_obj.tun_udata, dst_obj.tun_udata); 
}

SGWu& SGWu::operator=(SGWu src_obj) {

	swap(*this, src_obj);
	return *this;
}

SGWu::SGWu(SGWu &&src_obj)
	:SGWu() {

	swap(*this, src_obj);
}

uint16_t SGWu::generate_uteid(int &ue_number) {
	
	return ue_number;
}

void SGWu::set_uteid() {

	uteid = pkt.gtpu_hdr.uteid;
}

void SGWu::set_tun_udata(bool &data_invalid) {

	if (g_tun_utable.find(uteid) != g_tun_utable.end()) {
		tun_udata = g_tun_utable[uteid];
		data_invalid = false;
	}
	else {
		data_invalid = true;
	}
}

void SGWu::set_pgw_num() {

	if (socket_table.find(tun_udata.pgw_addr) != socket_table.end()) {
		num = socket_table[tun_udata.pgw_addr];
	}
	else {
		connect_with_pgw();
		num = socket_table[tun_udata.pgw_addr];
	}
}

void SGWu::connect_with_pgw() {

	to_pgw[pos].bind_client();
	to_pgw[pos].fill_server_details(tun_udata.pgw_port, tun_udata.pgw_addr.c_str());
	to_pgw[pos].connect_with_server(pos);
	handshake_with_pgw();
	socket_table[tun_udata.pgw_addr] = pos;
	pos++;	
}

void SGWu::handshake_with_pgw() {

	to_pgw[pos].pkt.clear_data();
	to_pgw[pos].pkt.fill_data(0, sizeof(int), type);
	to_pgw[pos].pkt.make_data_packet();
	to_pgw[pos].write_data();		
}

void SGWu::copy_data(Packet &arg) {

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

void SGWu::make_data_enodeb() {

	pkt.fill_gtpu_hdr(tun_udata.enodeb_uteid);
	pkt.add_gtpu_hdr();
}

void SGWu::make_data_pgw() {

	pkt.fill_gtpu_hdr(tun_udata.pgw_uteid);
	pkt.add_gtpu_hdr();
}

void SGWu::recv_enodeb(Server &sgw_server) {

	sgw_server.read_data();
	copy_data(sgw_server.pkt);
	pkt.rem_gtpu_hdr();
	cout << endl << "Received data from Enodeb successfully and removed the GTPu header" << endl;
}

void SGWu::send_enodeb(Server &sgw_server) {

	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, pkt.data_len, pkt.data);
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
	cout << "Sent data to Enodeb" << endl << endl;
}

void SGWu::recv_pgw(int &pgw_num) {
	struct ip *iphdr = allocate_ip_mem(20);
	struct tcphdr *tcp_hdr = allocate_tcp_mem(20);
	char *sink = allocate_str_mem(INET_ADDRSTRLEN);

	to_pgw[pgw_num].read_data();
	cout << "Received data from PGW" << endl;
	copy_data(to_pgw[pgw_num].pkt);
	pkt.rem_gtpu_hdr();

	memcpy(iphdr, pkt.data, 20 * sizeof(uint8_t));
	memcpy(tcp_hdr, pkt.data + 20 * sizeof(uint8_t), 20 * sizeof(uint8_t));	
	inet_ntop(AF_INET, &(iphdr->ip_dst), sink, INET_ADDRSTRLEN);
	// cout << endl << "UE IP is " << sink << endl;
	// cout << "TCP destination port is " << ntohs(tcp_hdr->th_dport) << endl;	
	// cout << "Received data from PGW successfully and removed the GTPu header" << endl;
	free(iphdr);
	free(tcp_hdr);
	free(sink);
}

void SGWu::send_pgw() {

	to_pgw[num].pkt.clear_data();
	to_pgw[num].pkt.fill_data(0, pkt.data_len, pkt.data);
	to_pgw[num].pkt.make_data_packet();
	to_pgw[num].write_data();
	cout << "Send data successfully to PGW with TEID - " << tun_udata.pgw_uteid << endl << endl;
}

void SGWu::fill_tun_utable(uint16_t &uteid, TunUdata &tun_udata) {

	g_tun_utable[uteid] = tun_udata;
}

void SGWu::erase_tun_utable(uint16_t &uteid) {

	// g_tun_utable.erase(uteid); /* This is to make the leftover data traffic to reach its intended destination  */
}

SGWu::~SGWu() {

	// Dummy
}
