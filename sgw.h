#ifndef SGW_H
#define SGW_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"

#define UDP_LINKS 10

struct TunData{
	uint16_t enodeb_uteid;
	uint16_t pgw_uteid;
	int pgw_port;
	char *pgw_addr;

	TunData();
	~TunData();
};

struct SGW{
	unordered_map<uint16_t, int> socket_table;
	vector<Client> to_pgw;
	Packet pkt;
	uint16_t uteid;
	int pos;
	int num;
	TunData tun_data;

	SGW();
	void get_cteid(int);
	void get_uteid(int);
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
	~SGW();
};

unordered_map<uint16_t, TunData> g_tun_table;

#endif //SGW_H