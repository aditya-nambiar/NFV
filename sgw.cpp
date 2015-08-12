#include "sgw.h"

void TunData::TunData(){
	
	mme_addr = allocate_str_mem(INET_ADDRSTRLEN);
	pgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
	enodeb_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

void TunData::~TunData(){
	
	free(mme_addr);
	free(pgw_addr);
	free(enodeb_addr);
}

void SGW::SGW(){
	
	socket_table.clear();
	to_pgw.resize(UDP_LINKS);
	pos = 0;
}

int SGW::get_cteid(int ue_num){
	
	return ue_num;
}

int SGW::get_uteid(int ue_num){
	
	return ue_num;
}

void SGW::set_uteid(){

	uteid = pkt.gtpu_hdr.teid;
}

void SGW::set_tun_data(){

	tun_data = g_tun_table[uteid];
}

void SGW::set_pgw_num(){

	if(socket_table.find(uteid) != socket_table.end())
		num = socket_table[uteid];
	else{
		connect_with_pgw();
		num = socket_table[uteid];
	}
}

void SGW::connect_with_pgw(){

	to_pgw[pos].bind_client();
	to_pgw[pos].fill_server_details(tun_data.pgw_port, tun_data.pgw_addr);
	to_pgw[pos].connect_with_server(pos);
	handshake_with_sgw();
	socket_table[uteid] = pos;
	pos++;	
}

void SGW::handshake_with_sgw(){
	int type = 2;

	to_pgw[pos].pkt.clear_data();
	to_pgw[pos].pkt.fill_data(0, sizeof(int), type);
	to_pgw[pos].pkt.make_data_packet();
	to_pgw[pos].write_data();		
}

void SGW::copy_data(Packet &arg){

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

void SGW::make_data_enodeb(){

	pkt.fill_gtpu_hdr(tun_data.enodeb_uteid);
	pkt.add_gtpu_hdr();
}

void SGW::make_data_pgw(){

	pkt.fill_gtpu_hdr(tun_data.pgw_uteid);
	pkt.add_gtpu_hdr();
}

void SGW::send_enodeb(Server &sgw_server){

	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, pkt.data_len. pkt.data);
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
}

void SGW::recv_enodeb(Server &sgw_server){

	sgw_server.read_data();
	copy_data(sgw_server.pkt);
	pkt.rem_gtpu_hdr();
}

void SGW::send_pgw(){

	to_pgw[num].pkt.clear_data();
	to_pgw[num].pkt.fill_data(0, pkt.data_len, pkt.data);
	to_pgw[num].pkt.make_data_packet();
	to_pgw[num].pkt.write_data();
}

void SGW::recv_pgw(){

	to_pgw[num].read_data();
	copy_data(to_pgw[num].pkt);
	pkt.rem_gtpu_hdr();
}

void SGW::~SGW(){

	// Dummy
}
