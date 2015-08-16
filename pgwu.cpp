#include "pgwu.h"

TunUdata::TunUdata(){

	// Dummy
};

TunUdata::~TunUdata(){

	// Dummy
}

PGWu::PGWu(){

	RawSocket::set_interface("lo");
	raw_client.bind_client();
}

uint16_t PGWu::generate_uteid(int &ue_num){

	return ue_num;
}

void PGWu::recv_sgw(Server &pgw_server){

	pgw_server.read_data();
	pgw_server.pkt.rem_gtpu_hdr();
	copy_to_rawpkt(pgw_server.pkt);
}

void PGWu::copy_to_rawpkt(Packet &arg){

	raw_client.pkt.clear_data();
	raw_client.pkt.fill_data(0, arg.data_len, arg.data);
}

void PGWu::send_raw_socket(){

	raw_client.write_data();
}

void PGWu::fill_tun_utable(char* ue_ip, TunUdata &tun_udata){

	g_tun_utable[ue_ip] = tun_udata;
}

PGWu::~PGWu(){

	// Dummy
}