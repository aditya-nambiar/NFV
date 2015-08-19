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
	void create_session_request_from_sgw(Server&, uint16_t&);
	void copy_to_pkt(Packet&);
	void set_ue_num();
	void set_bearer_id();
	void add_bearer_id();	
	void set_tun_cdata();
	void set_cteid();
	uint16_t generate_cteid(int&);	
	void create_session_response_to_sgw(Server&, uint16_t&);
	void fill_tun_ctable();	
	~PGWc();
};

extern vector<char*> g_ip_table;
extern unordered_map<int, int> g_bearer_table;
extern unordered_map<char*, TunCdata> g_tun_ctable;

void setup_ip_table();
void set_ip_table_size();
void fill_ip_table();
void free_ip_table();

#endif //PGWC_H