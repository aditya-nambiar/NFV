#ifndef ENODEB_H
#define ENODEB_H

#include "utils.h"
#include "packet.h"
#include "client.h"

#define UDP_LINKS 10

struct TunData {
	uint16_t sgw_uteid;
	int sgw_port;
	string sgw_addr;

	TunData();
	TunData(const TunData&);
	friend void swap(TunData&, TunData&);
	TunData& operator=(TunData);
	TunData(TunData&&);
	~TunData();
};

struct EnodeB {
	unordered_map<string, int> socket_table;
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
	EnodeB(const EnodeB&);
	friend void swap(EnodeB&, EnodeB&);
	EnodeB& operator=(EnodeB);
	EnodeB(EnodeB&&);
	uint16_t generate_uteid(int);
	void attach_to_tun();
	void read_tun();
	void write_tun();
	void set_ue_ip();
	void set_tun_data(bool&);
	void set_sgw_num();
	void connect_with_sgw();
	void handshake_with_sgw();
	void make_data();
	void send_data();
	void recv_data(int&);
	void fill_tun_table(string&, TunData&);
	void erase_tun_table(string&);
	~EnodeB();
};

extern unordered_map<string, TunData> g_tun_table;

#endif //ENODEB_H
