#ifndef MME_H
#define MME_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "client.h"

unordered_map<int, int> g_bearer_table;

struct Gateways{
	int sgw_port;
	int pgw_port;
	char *sgw_addr;
	char *pgw_addr;

	Gateways();
	void set_sgw();
	void set_pgw();
	~Gateways();
};

struct Tunnel{
	uint16_t mme_cteid;
	uint16_t sgw_cteid;
	uint16_t enodeb_uteid;
	uint16_t sgw_uteid;

	void set_mme_cteid(int);
};

void* multithreading_func(void*);
void authenticate(Server&, Client&);
void setup_tunnel(Server&, Client&, int);
void create_session(Client&, int, Tunnel&);
void modify_session(Client&, int, Tunnel&);
void set_bearer_id(int);
int generate_bearer_id(int);

#endif //MME_H