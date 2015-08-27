#include "mme.h"

TunData::TunData(){

	sgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
	pgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

TunData::~TunData(){

	free(sgw_addr);
	free(pgw_addr);
}

MME::MME(){

	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	reply = allocate_str_mem(BUFFER_SIZE);
}

void MME::set_ue_num(){


}

void MME::set_cteid(){

	cteid = generate_cteid(ue_num);
}

uint16_t MME::generate_cteid(int &ue_number){

	return ue_number;
}

MME::~MME(){

	free(ue_ip);
	free(reply);
}