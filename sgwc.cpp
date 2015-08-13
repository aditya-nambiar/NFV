#include "sgwc.h"

TunCdata::TunCdata(){

	pgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

TunCdata::~TunCdata(){

	free(pgw_addr);
}

SGWc::SGWc(){

	ue_ip = allocate_str_mem(INET_ADDRSTRLEN)
	reply = allocate_str_mem(BUFFER_SIZE);
}

uint16_t SGWc::get_cteid(int ue_num){

	return ue_num;
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

void SGWc::copy_data(Packet &arg){

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

void SGWc::connect_with_pgw(){

	to_pgw.bind_client();
	to_pgw.fill_server_details(g_pgw_port, g_pgw_addr);
	to_pgw.connect_with_server(ue_num);
}

SGWc::~SGWc(){

	free(ue_ip);
	free(reply);
}