#ifndef SGWC_H
#define SGWC_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"

struct TunCdata{
	uint16_t mme_cteid;
	uint16_t pgw_cteid;
	int pgw_port;
	char *pgw_addr;

	TunCdata();
	~TunCdata();
};

struct SGWc{
	Client to_pgw;
	Packet pkt;
	int ue_num;
	uint16_t cteid;
	int bearer_id;
	int type;
	char *ue_ip;
	char *reply;
	TunCdata tun_cdata;

	SGWc();
	void create_session_request_from_mme(Server&);
	void copy_data(Packet&);
	void set_ue_num();
	void set_bearer_id();
	void add_bearer_id();
	void set_cteid();
	void connect_with_pgw();
	void handshake_with_pgw();
	void create_session_request_to_pgw(uint16_t&);
	void create_session_response_from_pgw(uint16_t&);
	void set_tun_cdata();
	void set_ue_ip();
	void create_session_response_to_mme(Server&);
	void modify_session_request_from_mme(uint16_t&);
	void modify_session_response_to_mme(Server, uint16_t);
	~SGWc();
};

unordered_map<int, int> g_bearer_table;
unordered_map<uint16_t, TunCdata> g_tun_ctable;

#endif //SGWC_H