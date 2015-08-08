#ifndef RAN_H
#define RAN_H

#include "utils.h"
#include "packet.h"
#include "client.h"
#include "ue.h"

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
	string ue_ip;

	void attach_to_tun();
	void set_ue_ip();
	int get_sgw_num();
	void connect_with_sgw();


};

unordered_map<char*, TunData> g_tun_table;

void setup_tun();
void* monitor_traffic(void*);
void* multithreading_func(void*);
void attach_with_mme(UserEquipment&, Client&);
void send_traffic(UserEquipment&, int&);

#endif //RAN_H