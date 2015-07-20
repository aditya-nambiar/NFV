#ifndef SGW_H
#define SGW_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"

unordered_map<int, int> g_bearer_table;

struct Tunnel{
	uint16_t sgw_cteid;
	uint16_t sgw_uteid;
	uint16_t mme_cteid;
	uint16_t pgw_cteid;
	uint16_t pgw_uteid;
	uint16_t enodeb_uteid;

	void set_sgw_cteid(int);
	void set_sgw_uteid(int);
};

void* multithreading_func(void*);
void handle_cdata(Server&, Tunnel&, int&);
void handle_udata(Server&, Tunnel&, int&);
void set_bearer_id(int, int);

#endif //SGW_H