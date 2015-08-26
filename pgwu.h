#ifndef PGWU_H
#define PGWU_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "raw_socket.h"

struct TunUdata{
	uint16_t sgw_uteid;

	TunUdata();
	~TunUdata();
};

struct PGWu{
	RawSocket raw_client;
	Server for_sink;
	char *ue_ip;
	TunUdata tun_udata;

	PGWu();
	uint16_t generate_uteid(int&);
	void configure_raw_client();
	void configure_server_for_sink();
	void set_ue_ip();
	void set_tun_udata();
	void recv_sgw(Server&);
	void copy_to_rawpkt(Packet&);
	void send_sgw(Server&);
	void copy_sinkpkt_to_pgwpkt(Packet&);
	void send_raw_socket();
	void recv_sink();
	void fill_tun_utable(string, TunUdata&);
	~PGWu();
};

extern unordered_map<string, TunUdata> g_tun_utable;

#endif //PGWU_H
