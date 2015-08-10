#ifndef SGW_H
#define SGW_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"

unordered_map<int, int> g_bearer_table;
unordered_map<uint16_t, uint16_t> g_tun_table;

struct TunData{
	uint16_t mme_cteid;
	uint16_t pgw_cteid;
	uint16_t pgw_uteid;
	uint16_t enodeb_uteid;
	int mme_port;
	int pgw_port;
	int enodeb_port;
	char *mme_addr;
	char *pgw_addr;
	char *enodeb_addr;

	TunData();
	~TunData();
};

struct SGW{
	uint16_t cteid;
	uint16_t uteid;
	int port;
	char *ip_addr;
	Packet pkt;

	SGW();
	void set_cteid(int);
	void set_uteid(int);
	~SGW();
};

void* process_traffic(void*);
void handle_cdata(Server&, Tunnel&, int&);
void handle_udata(Server&, Tunnel&, int&);
void set_bearer_id(int, int);

#endif //SGW_H