#ifndef MME_H
#define MME_H

#include "utils.h"
#include "packet.h"

struct TunData{
	uint16_t sgw_cteid;
	int sgw_port;
	int pgw_port;
	char *sgw_addr;
	char *pgw_addr;

	TunData();
	~TunData();
};

struct MME{
	uint16_t cteid;
	int ue_num;
	char *ue_ip;
	char *reply;

	MME();
	void set_ue_num();
	void set_cteid();
	uint16_t generate_cteid(int&);
	~MME();
};


#endif //MME_H