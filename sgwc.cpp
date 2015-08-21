#include "sgwc.h"

unordered_map<int, int> g_bearer_table;
unordered_map<uint16_t, TunCdata> g_tun_ctable;

TunCdata::TunCdata(){


}

TunCdata::~TunCdata(){


}

SGWc::SGWc(){

	type = 1;
	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	reply = allocate_str_mem(BUFFER_SIZE);
}

void SGWc::create_session_request_from_mme(Server &sgw_server){

	copy_data(sgw_server.pkt);
	set_ue_num();
	set_bearer_id();
	set_cteid();
}

void SGWc::copy_data(Packet &arg){

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

void SGWc::set_ue_num(){

	memcpy(&ue_num, pkt.data + sizeof(int), sizeof(int));
}

void SGWc::set_bearer_id(){

	memcpy(&bearer_id, pkt.data + 2*sizeof(int), sizeof(int));
	add_bearer_id();
}

void SGWc::add_bearer_id(){

	g_bearer_table[ue_num] = bearer_id;
}

void SGWc::set_cteid(){

	cteid = generate_cteid(ue_num);
}

uint16_t SGWc::generate_cteid(int &ue_num){

	return ue_num;
}

void SGWc::create_session_request_to_pgw(uint16_t &uteid){

	connect_with_pgw();
	to_pgw.pkt.clear_data();
	to_pgw.pkt.fill_data(0, sizeof(int), ue_num);
	to_pgw.pkt.fill_data(sizeof(int), sizeof(int), bearer_id);
	to_pgw.pkt.fill_data(2*sizeof(int), sizeof(uint16_t), cteid);
	to_pgw.pkt.fill_data(2*sizeof(int) + sizeof(uint16_t), sizeof(uint16_t), uteid);
	to_pgw.pkt.make_data_packet();
	to_pgw.write_data();
}

void SGWc::connect_with_pgw(){

	to_pgw.bind_client();
	to_pgw.fill_server_details(g_pgw_port, g_pgw_addr);
	to_pgw.connect_with_server(ue_num);
	handshake_with_pgw();
}

void SGWc::handshake_with_pgw(){

	to_pgw.pkt.clear_data();
	to_pgw.pkt.fill_data(0, sizeof(int), type);
	to_pgw.pkt.make_data_packet();
	to_pgw.write_data();
}

void SGWc::create_session_response_from_pgw(uint16_t &pgw_uteid){

	to_pgw.read_data();
	to_pgw.pkt.rem_gtpc_hdr();
	set_tun_cdata();
	memcpy(&pgw_uteid, to_pgw.pkt.data + sizeof(uint16_t), sizeof(uint16_t));
	set_ue_ip();	
}

void SGWc::set_tun_cdata(){

	memcpy(&tun_cdata.mme_cteid, pkt.data + 3*sizeof(int), sizeof(uint16_t));
	memcpy(&tun_cdata.pgw_cteid, to_pgw.pkt.data, sizeof(uint16_t));
	tun_cdata.pgw_port = g_pgw_port;
	tun_cdata.pgw_addr.assign(g_pgw_addr);
}

void SGWc::set_ue_ip(){

	memcpy(ue_ip, to_pgw.pkt.data + 2*sizeof(uint16_t), INET_ADDRSTRLEN);		
}

void SGWc::create_session_response_to_mme(Server &sgw_server){

	strcpy(reply, "OK");
	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, sizeof(uint16_t), cteid);
	sgw_server.pkt.fill_data(sizeof(uint16_t), strlen(reply), reply);
	sgw_server.pkt.fill_gtpc_hdr(tun_cdata.mme_cteid);
	sgw_server.pkt.add_gtpc_hdr();
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
}

void SGWc::modify_session_request_from_mme(Server &sgw_server, uint16_t &enodeb_uteid){

	sgw_server.read_data();
	sgw_server.pkt.rem_gtpc_hdr();
	copy_data(sgw_server.pkt);
	memcpy(&enodeb_uteid, pkt.data, sizeof(uint16_t));
}

void SGWc::modify_session_response_to_mme(Server &sgw_server, uint16_t &sgw_uteid){

	strcpy(reply, "OK");
	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, sizeof(uint16_t), sgw_uteid);
	sgw_server.pkt.fill_data(sizeof(uint16_t), INET_ADDRSTRLEN, ue_ip);
	sgw_server.pkt.fill_data(sizeof(uint16_t) + INET_ADDRSTRLEN, strlen(reply), reply);
	sgw_server.pkt.fill_gtpc_hdr(tun_cdata.mme_cteid);
	sgw_server.pkt.add_gtpc_hdr();
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
}

void SGWc::fill_pgw_addr(int &pgw_port, string &pgw_addr){

	pgw_port = tun_cdata.pgw_port;
	pgw_addr = tun_cdata.pgw_addr;
}

void SGWc::fill_tun_ctable(){

	g_tun_ctable[cteid] = tun_cdata;
}

SGWc::~SGWc(){

	free(ue_ip);
	free(reply);
}