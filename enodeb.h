#ifndef ENODEB_H
#define ENODEB_H

#include "utils.h"
#include "packet.h"
#include "client.h"

#define UDP_LINKS 10

struct TunData{
	uint16_t sgw_uteid;
	int sgw_port;
	char *sgw_addr;

	TunData();
	~TunData();
};

struct EnodeB{
	unordered_map<char*, int> socket_table;
	vector<Client> to_sgw;
	Packet pkt;
	int tun_fd;
	int pos;
	int num;
	int count;
	char *ue_ip;
	char *tun_name;
	TunData tun_data;

	EnodeB();
	uint16_t generate_uteid(int);
	void attach_to_tun();
	void read_tun();
	void write_tun();
	void set_ue_ip();
	void set_tun_data();
	void set_sgw_num();
	void connect_with_sgw();
	void handshake_with_sgw();
	void make_data();
	void send_data();
	void recv_data();
	~EnodeB();
};

extern unordered_map<char*, TunData> g_tun_table;

#endif //ENODEB_H