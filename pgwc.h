#ifndef PGWC_H
#define PGWC_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "client.h"

#define MAX_IPS 200

struct TunCdata {
	uint16_t sgw_cteid;

	TunCdata();
	TunCdata(const TunCdata&);
	friend void swap(TunCdata&, TunCdata&);
	TunCdata& operator=(TunCdata);
	TunCdata(TunCdata&&);
	~TunCdata();
};

struct PGWc {
	Packet pkt;
	int ue_num;
	int bearer_id;
	uint16_t cteid;
	TunCdata tun_cdata;
	char *reply;

	PGWc();
	PGWc(const PGWc&);
	friend void swap(PGWc&, PGWc&);
	PGWc& operator=(PGWc);
	PGWc(PGWc&&);
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
	void delete_session_req_from_sgw(Server&);
	void delete_session_res_to_sgw(Server&);
	void erase_bearer_table();
	void erase_tun_ctable();
	~PGWc();
};

extern vector<string> g_ip_table;
extern unordered_map<int, int> g_bearer_table;
extern unordered_map<string, TunCdata> g_tun_ctable;

void setup_ip_table();
void set_ip_table_size();
void fill_ip_table();

#endif //PGWC_H
