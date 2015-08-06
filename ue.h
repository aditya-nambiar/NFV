#ifndef UE_H
#define UE_H

#include "utils.h"
#include "packet.h"
#include "client.h"

struct TrafficGenerator{
	char *data;
	char *src_ip;
	char *dst_ip;
	int src_port;
	int dst_port;

	TrafficGenerator();
	void fill_traffic();
	~TrafficGenerator();
};

class UserEquipment{
public:
	//UE data
	int ue_num;
	unsigned long long key;
	unsigned long long imsi;
	unsigned long long msisdn;
	char *ip_addr;

	//eNodeB data
	uint16_t enodeb_uteid;
	uint16_t sgw_uteid;

	int type;
	
	UserEquipment(int);
	unsigned long long key_generation(int);		
	void set_enodeb_uteid(int);
	unsigned long long get_autn_res(unsigned long long, unsigned long long);	
	void authenticate(Client&);
	void setup_tunnel(Client&);
	void setup_interface();
	void send_traffic(Client&);
	~UserEquipment();		
};

#endif //UE_H