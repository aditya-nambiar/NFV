#ifndef MME_H
#define MME_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "thread_pool.h"
#include "client.h"

struct TunData {
	uint16_t mme_cteid;
	uint16_t sgw_cteid;
	uint16_t sgw_uteid;
	uint16_t enodeb_uteid;
	int sgw_port;
	int pgw_port;
	char *sgw_addr;
	char *pgw_addr;

	TunData();
	TunData(const TunData&);
	friend void swap(TunData&, TunData&);
	TunData& operator=(TunData);
	TunData(TunData&&);
	~TunData();
};

struct MME {	
	Server mme_server;
	Client to_hss;
	Client to_sgw;
	int ue_num;
	int bearer_id;
	int type;
	char *ue_ip;
	string reply;
	unsigned long long autn_num;
	unsigned long long rand_num;
	unsigned long long autn_xres;
	unsigned long long autn_res;
	TunData tun_data;

	MME();
	MME(const MME&);
	friend void swap(MME&, MME&);
	MME& operator=(MME);
	MME(MME&&);
	void set_cteid();
	uint16_t generate_cteid(int&);
	void set_bearer_id();
	int generate_bearer_id(int&);
	void set_sgw();
	void set_pgw();
	void startup_mme_server(ClientDetails&);
	void set_ue_num();
	void attach_req_from_ue();
	void setup_hss_client();
	void fetch_ue_data();
	void authenticate_ue();
	void setup_sgw_client();
	void create_session_req_to_sgw();
	void create_session_res_from_sgw();
	void recv_enodeb();
	void modify_session_req_to_sgw();
	void modify_session_res_from_sgw();
	void send_enodeb();
	void detach_req_from_ue();
	void delete_session_req_to_sgw();
	void delete_session_res_from_sgw();
	void detach_res_to_ue();
	void rem_bearer_id();
	void rem_tun_data();
	~MME();
};

#endif //MME_H