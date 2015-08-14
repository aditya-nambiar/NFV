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
	Packet pkt;
	RawSocket raw_socket;

	PGWu();
	void recv_sgw(Server&);
	~PGWu();
};

unordered_map<char*, TunUdata> g_tun_utable;

#endif //PGWU_H