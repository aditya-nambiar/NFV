#include "client.h"

class UserEquipment{
	public:
		unsigned long long key;
		unsigned long long imsi;
		unsigned long long msisdn;
		
		int type;
		
		UserEquipment(int);
		unsigned long long key_generation(int);
		void authenticate(Client&);
		void process_autn_request(Client&);
		void process_autn_response(Client&);
		unsigned long long get_autn_res(unsigned long long,unsigned long long);
		~UserEquipment();		
};
