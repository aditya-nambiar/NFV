#ifndef ENODEB_H
#define ENODEB_H

#include "utils.h"
#include "packet.h"
#include "client.h"

#define UDP_LINKS 10

struct TunData{
	uint16_t sgw_uteid;
	char *sgw_addr;
	int sgw_port;

	TunData();
	~TunData();
};

struct EnodeB{
	unordered_map<char*, int> socket_table;
	vector<Client> to_sgw(UDP_LINKS);	
	int status;
	int tun_fd;
	int pos;
	int num;
	int count;
	Packet pkt;
	TunData tun_data;
	char* ue_ip;

	EnodeB();
	void attach_to_tun();
	void read_tun();
	void write_tun();
	void set_ue_ip();
	int set_sgw_num();
	void connect_with_sgw();
	void send_data();
	void recv_data();
	~EnodeB();
};

#endif //ENODEB_H