#ifndef PGW_H
#define PGW_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "raw_socket.h"

unordered_map<int, int> g_bearer_table;
vector<char*> g_static_ips(MAX_CONNECTIONS, NULL);

struct Tunnel{
	uint16_t sgw_cteid;
	uint16_t sgw_uteid;
	uint16_t pgw_cteid;
	uint16_t pgw_uteid;

	void set_pgw_cteid(int);
	void set_pgw_uteid(int);
};

void set_static_ips();
void* multithreading_func(void*);
void handle_cdata(Server&, Tunnel&, int&);
void handle_udata(Server&, Tunnel&, int&);
void set_bearer_id(int, int);
void free_static_ips();


#endif //PGW_H