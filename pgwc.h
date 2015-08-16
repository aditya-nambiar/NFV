#ifndef PGWC_H
#define PGWC_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"

#define MAX_IPS 200

struct TunCdata{
	uint16_t sgw_cteid;

	TunCdata();
	~TunCdata();
};

struct PGWc{
	Packet pkt;
	int ue_num;
	int bearer_id;
	uint16_t cteid;
	TunCdata tun_cdata;

	PGWc();
	void create_session_request_from_sgw(Server&);
	void copy_to_pkt(Packet&);
	void set_ue_num();
	void set_bearer_id();
	void add_bearer_id();	
	void set_tun_cdata();
	void set_cteid();
	uint16_t generate_cteid(int&);	
	void create_session_response_to_sgw(Server&);
	void fill_tun_ctable();	
	~PGWc();
};

vector<char*> g_ip_table;
unordered_map<int, int> g_bearer_table;
unordered_map<uint16_t, TunCdata> g_tun_ctable;

void set_ip_table();
void set_ip_table_size();
void fill_ip_table();
void free_ip_table();

#endif //PGWC_H