#ifndef UE_H
#define UE_H

#include "utils.h"
#include "packet.h"
#include "client.h"

class UserEquipment{
public:
	//UE data
	unsigned long long key;
	unsigned long long imsi;
	unsigned long long msisdn;
	char *ip_addr;

	//eNodeB data
	uint16_t enodeb_uteid;
	uint16_t dst_uteid;

	int type;
	
	UserEquipment(int);
	unsigned long long key_generation(int);		
	uint16_t generate_uteid();
	unsigned long long get_autn_res(unsigned long long, unsigned long long);	
	void authenticate(Client&);
	void setup_tunnel(Client&);
	~UserEquipment();		
};

#endif //UE_H