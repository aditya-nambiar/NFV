#ifndef MME_SERVER_H
#define MME_SERVER_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "client.h"
#include "mme.h"

extern unordered_map<int, int> g_bearer_table;
extern double g_req_duration;
extern time_t g_start_time;

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
void setup_tunnel(Server&, Client&, int&, Tunnel&);
void detach(Server&, Client&, Tunnel&, int&);
void create_session(Client&, int, Tunnel&);
void modify_session(Client&, int, Tunnel&);
void set_bearer_id(int);
int generate_bearer_id(int);
void detach_req_from_ran(Server&, int&);
void delete_session_req_to_sgw(Server&, Client&, Tunnel&);
void delete_bearer_id(int&);
void delete_session_res_from_sgw(Server&, Client&, int&);
void detach_res(Server&, int&);
void startup_mme(char *argv[]);

#endif //MME_SERVER_H