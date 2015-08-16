#ifndef PGWU_H
#define PGWU_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"

struct TunUdata{
	uint16_t sgw_uteid;

	TunUdata();
	~TunUdata();
};

struct PGWu{
	RawSocket raw_client;

	PGWu();
	uint16_t generate_uteid(int&);
	void recv_sgw(Server&);
	void copy_to_rawpkt(Packet&);
	void send_raw_socket();
	void fill_tun_utable(char*, TunUdata&)
	~PGWu();
};

unordered_map<char*, TunUdata> g_tun_utable;

#endif //PGWU_H