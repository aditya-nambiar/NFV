#ifndef SGWC_H
#define SGWC_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "client.h"

struct TunCdata {
	uint16_t mme_cteid;
	uint16_t pgw_cteid;
	int pgw_port;
	string pgw_addr;

	TunCdata();
	TunCdata(const TunCdata&);
	friend void swap(TunCdata&, TunCdata&);
	TunCdata& operator=(TunCdata);
	TunCdata(TunCdata&&);
	~TunCdata();
};

struct SGWc {
	Client to_pgw;
	Packet pkt;
	int ue_num;
	uint16_t cteid;
	int bearer_id;
	int type;
	char *ue_ip;
	string reply;
	TunCdata tun_cdata;

	SGWc();
	SGWc(const SGWc&);
	friend void swap(SGWc&, SGWc&);
	SGWc& operator=(SGWc);
	SGWc(SGWc&&);
	void create_session_req_from_mme(Server&);
	void copy_data(Packet&);
	void set_ue_num();
	void set_bearer_id();
	void fill_bearer_table();
	void set_cteid();
	uint16_t generate_cteid(int&);
	void create_session_req_to_pgw(uint16_t&);
	void connect_with_pgw();
	void handshake_with_pgw();
	void create_session_res_from_pgw(uint16_t&);
	void set_tun_cdata();
	void set_ue_ip();
	void create_session_res_to_mme(Server&);
	void modify_session_req_from_mme(Server&, uint16_t&);
	void modify_session_res_to_mme(Server&, uint16_t&);
	void fill_pgw_addr(int&, string&);
	void fill_tun_ctable();
	void delete_session_req_from_mme(Server&);
	void copy_pkts(Packet&, Packet&);
	void delete_session_req_to_pgw();
	void erase_bearer_table();
	void delete_session_res_from_pgw();
	void delete_session_res_to_mme(Server&);
	void erase_tun_ctable();

	~SGWc();
};

extern unordered_map<int, int> g_bearer_table;
extern unordered_map<uint16_t, TunCdata> g_tun_ctable;

#endif //SGWC_H