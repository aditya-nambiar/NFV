#ifndef SGWU_H
#define SGWU_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "client.h"

#define UDP_LINKS 10

struct TunUdata{
	uint16_t enodeb_uteid;
	uint16_t pgw_uteid;
	int pgw_port;
	string pgw_addr;

	TunUdata();
	~TunUdata();
};

struct SGWu{
	unordered_map<uint16_t, int> socket_table;
	vector<Client> to_pgw;
	Packet pkt;
	uint16_t uteid;
	int pos;
	int num;
	int type;
	TunUdata tun_udata;

	SGWu();
	uint16_t generate_uteid(int&);
	void set_uteid();
	void set_tun_udata();
	void set_pgw_num();
	void connect_with_pgw();
	void handshake_with_pgw();
	void copy_data(Packet&);
	void make_data_enodeb();
	void make_data_pgw();
	void recv_enodeb(Server&);
	void send_enodeb(Server&);
	void recv_pgw(int&);
	void send_pgw();
	void fill_tun_utable(uint16_t&, TunUdata&);
	~SGWu();
};

extern unordered_map<uint16_t, TunUdata> g_tun_utable;

#endif //SGWU_H