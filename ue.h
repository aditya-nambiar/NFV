#ifndef UE_H
#define UE_H

#include "utils.h"
#include "packet.h"
#include "client.h"

class UserEquipment{
public:
	unsigned long long key;
	unsigned long long imsi;
	unsigned long long msisdn;
	
	int type;
	
	UserEquipment(int);
	unsigned long long key_generation(int);		
	unsigned long long get_autn_res(unsigned long long, unsigned long long);	
	void authenticate(Client&);
	void setup_tunnel(Client&);
	~UserEquipment();		
};

#endif //UE_H