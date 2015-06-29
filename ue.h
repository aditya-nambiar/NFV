#include "client.h"

class UserEquipment{
	public:
		unsigned long long key;
		unsigned long long imsi;
		unsigned long long msisdn;
		
		int type;
		
		UserEquipment(int);
		void authenticate(Client&);
		unsigned long long key_generation(int);		
		unsigned long long get_autn_res(unsigned long long, unsigned long long);
		~UserEquipment();		
};
