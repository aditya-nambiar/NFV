#include "pgwc.h"

vector<char*> g_ip_table;
unordered_map<int, int> g_bearer_table;
unordered_map<char*, TunCdata> g_tun_ctable;

TunCdata::TunCdata(){

	// Dummy
}

TunCdata::~TunCdata(){

	// Dummy
}

PGWc::PGWc(){

	// Dummy
}

void PGWc::create_session_request_from_sgw(Server &pgw_server, uint16_t &sgw_uteid){

	copy_to_pkt(pgw_server.pkt);
	set_ue_num();
	set_bearer_id();
	add_bearer_id();
	set_tun_cdata();
	memcpy(&sgw_uteid, pkt.data + 2*sizeof(int) + sizeof(uint16_t), sizeof(uint16_t));
}

void PGWc::copy_to_pkt(Packet &arg){

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

void PGWc::set_ue_num(){

	memcpy(&ue_num, pkt.data, sizeof(int));
}

void PGWc::set_bearer_id(){

	memcpy(&bearer_id, pkt.data + sizeof(int), sizeof(int));
	add_bearer_id();
}

void PGWc::add_bearer_id(){

	g_bearer_table[ue_num] = bearer_id;
}

void PGWc::set_tun_cdata(){

	memcpy(&tun_cdata.sgw_cteid, pkt.data + 2*sizeof(int), sizeof(uint16_t));
}

void PGWc::set_cteid(){

	cteid = generate_cteid(ue_num);
}

uint16_t PGWc::generate_cteid(int &ue_num){

	return ue_num;
}

void PGWc::create_session_response_to_sgw(Server &pgw_server, uint16_t &uteid){

	pgw_server.pkt.clear_data();
	pgw_server.pkt.fill_data(0, sizeof(uint16_t), cteid);
	pgw_server.pkt.fill_data(sizeof(uint16_t), sizeof(uint16_t), uteid);
	pgw_server.pkt.fill_data(2*sizeof(uint16_t), INET_ADDRSTRLEN, g_ip_table[ue_num]);
	pgw_server.pkt.fill_gtpc_hdr(tun_cdata.sgw_cteid);
	pgw_server.pkt.add_gtpu_hdr();
	pgw_server.pkt.make_data_packet();
	pgw_server.write_data();
}

void PGWc::fill_tun_ctable(){

	g_tun_ctable[g_ip_table[ue_num]] = tun_cdata;
}

PGWc::~PGWc(){

	// Dummy
}

void setup_ip_table(){

	set_ip_table_size();
	fill_ip_table();
}

void set_ip_table_size(){

	g_ip_table.resize(MAX_IPS, NULL);
}

void fill_ip_table(){
	string prefix;
	string ip;
	int size;
	int i;
	
	prefix = "192.168.100.";
	size = g_ip_table.size();
	for(i=0;i<size;i++){
		ip = prefix + to_string(i+3);
		g_ip_table[i] = allocate_str_mem(INET_ADDRSTRLEN);
		strcpy(g_ip_table[i], ip.c_str());
	}
}

void free_ip_table(){
	int i;
	int size;

	size = g_ip_table.size();
	for(i=0;i<size;i++){
		free(g_ip_table[i]);
	}
}