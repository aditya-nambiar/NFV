#ifndef UE_H
#define UE_H

#include "utils.h"
#include "packet.h"
#include "client.h"

class UE {
public:
	// UE data
	int num;
	unsigned long long key;
	unsigned long long imsi;
	unsigned long long msisdn;
	char *ip_addr;
	char *res;
	string ip_addr_str;
	string res_str();
	Packet pkt;
	char *reply;
	
	// Connection data
	int type;

	// Sink data
	int sink_port;
	char *sink_addr;
	
	UE(int);
	UE(const UE&);
	friend void swap(UE&, UE&);
	UE& operator=(UE);
	UE(UE&&);
	unsigned long long generate_key(int);		
	void authenticate(Client&);
	unsigned long long get_autn_res(unsigned long long, unsigned long long);	
	void setup_tunnel(Client&, uint16_t&, uint16_t&, int&, string&);
	void send_traffic();
	void setup_interface();	
	void set_sink();
	void generate_data();
	void send_detach_req(Client&);
	void recv_detach_res(Client&);
	~UE();		
};

#endif //UE_H
