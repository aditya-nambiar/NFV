#include "sgwu.h"

void TunUdata::TunUdata(){
	
	pgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

void TunUdata::~TunUdata(){
	
	free(pgw_addr);
}

void SGWu::SGWu(){
	
	socket_table.clear();
	to_pgw.resize(UDP_LINKS);
	pos = 0;
	type = 2;
}

uint16_t SGWu::get_uteid(int &ue_num){
	
	return ue_num;
}

void SGWu::set_uteid(){

	uteid = pkt.gtpu_hdr.uteid;
}

void SGWu::set_tun_udata(){

	tun_udata = g_tun_utable[uteid];
}

void SGWu::set_pgw_num(){

	if(socket_table.find(uteid) != socket_table.end())
		num = socket_table[uteid];
	else{
		connect_with_pgw();
		num = socket_table[uteid];
	}
}

void SGWu::connect_with_pgw(){

	to_pgw[pos].bind_client();
	to_pgw[pos].fill_server_details(tun_udata.pgw_port, tun_udata.pgw_addr);
	to_pgw[pos].connect_with_server(pos);
	handshake_with_sgw();
	socket_table[uteid] = pos;
	pos++;	
}

void SGWu::handshake_with_sgw(){

	to_pgw[pos].pkt.clear_data();
	to_pgw[pos].pkt.fill_data(0, sizeof(int), type);
	to_pgw[pos].pkt.make_data_packet();
	to_pgw[pos].write_data();		
}

void SGWu::copy_data(Packet &arg){

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

void SGWu::make_data_enodeb(){

	pkt.fill_gtpu_hdr(tun_udata.enodeb.uteid);
	pkt.add_gtpu_hdr();
}

void SGWu::make_data_pgw(){

	pkt.fill_gtpu_hdr(tun_udata.pgw_uteid);
	pkt.add_gtpu_hdr();
}

void SGWu::send_enodeb(Server &sgw_server){

	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, pkt.data_len. pkt.data);
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
}

void SGWu::recv_enodeb(Server &sgw_server){

	sgw_server.read_data();
	copy_data(sgw_server.pkt);
	pkt.rem_gtpu_hdr();
}

void SGWu::send_pgw(){

	to_pgw[num].pkt.clear_data();
	to_pgw[num].pkt.fill_data(0, pkt.data_len, pkt.data);
	to_pgw[num].pkt.make_data_packet();
	to_pgw[num].pkt.write_data();
}

void SGWu::recv_pgw(){

	to_pgw[num].read_data();
	copy_data(to_pgw[num].pkt);
	pkt.rem_gtpu_hdr();
}

void fill_tunu_table(uint16_t &uteid, TunUdata &tun_udata){

	g_tun_utable[uteid] = tun_udata;
}

void SGWu::~SGWu(){

	// Dummy
}