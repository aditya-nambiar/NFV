#include "sgwc.h"

TunCdata::TunCdata(){

	pgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

TunCdata::~TunCdata(){

	free(pgw_addr);
}

SGWc::SGWc(){

	type = 1;
	ue_ip = allocate_str_mem(INET_ADDRSTRLEN)
	reply = allocate_str_mem(BUFFER_SIZE);
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

uint16_t SGWc::set_cteid(){

	return ue_num;
}

void SGWc::copy_data(Packet &arg){

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
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

void create_session_request_to_pgw(){

	to_pgw.pkt.clear_data();
	to_pgw.pkt.fill_data(0, sizeof(int), ue_num);
	to_pgw.pkt.fill_data(sizeof(int), sizeof(int), bearer_id);
	to_pgw.pkt.fill_data(2*sizeof(int), sizeof(uint16_t), sgw_cteid);
	to_pgw.pkt.fill_data(2*sizeof(int) + sizeof(uint16_t), sizeof(uint16_t), sgw_uteid);
	to_pgw.pkt.make_data_packet();
	to_pgw.write_data();
	to_pgw.read_data();
	to_pgw.pkt.rem_gtpc_hdr();




	to_pgw.pkt.fill_data(sizeof(int), sizeof(int), ue_num);
	to_pgw.pkt.fill_data(2*sizeof(int), sizeof(int), bearer_id);
	to_pgw.pkt.fill_data(3*sizeof(int), sizeof(uint16_t), sgw_cteid);
	to_pgw.pkt.fill_data(3*sizeof(int) + sizeof(uint16_t), sizeof(uint16_t), sgw_uteid);
	to_pgw.pkt.make_data_packet();
	to_pgw.write_data();
	to_pgw.read_data();
	to_pgw.pkt.rem_gtpc_hdr();
	memcpy(&tun_data.pgw_cteid, to_pgw.pkt.data, sizeof(uint16_t));
	memcpy(&tun_data.pgw_uteid, to_pgw.pkt.data + sizeof(uint16_t), sizeof(uint16_t));
	memcpy(ue_ip_addr, to_pgw.pkt.data + 2*sizeof(uint16_t), INET_ADDRSTRLEN);	
}

SGWc::~SGWc(){

	free(ue_ip);
	free(reply);
}