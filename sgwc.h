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
	void set_ue_num();
	void set_bearer_id();
	void add_bearer_id();
	uint16_t set_cteid();
	void copy_data(Packet&);
	void connect_with_pgw();
	void handshake_with_pgw();
	void create_session_request_from_mme();
	void create_session_request_to_pgw();
	void 
	~SGWc();







	unordered_map<uint16_t, int> socket_table;
	vector<Client> to_pgw;
	Packet pkt;
	uint16_t uteid;
	int pos;
	int num;
	TunCdata tun_udata;

	SGWc();
	uint16_t get_cteid(int);
	uint16_t get_uteid(int);
	void set_uteid(uint16_t);
	void set_pgw_num();
	void connect_with_pgw();
	void handshake_with_pgw();
	void copy_data(Packet&);
	void make_data();
	void send_enodeb(Server);
	void recv_enodeb(Server);
	void send_pgw();
	void recv_pgw();
	~SGWc();
};

unordered_map<int, int> g_bearer_table;
unordered_map<uint16_t, TunCdata> g_tun_ctable;

#endif //SGWC_H