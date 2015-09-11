#ifndef HSS_H
#define HSS_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "db_mysql.h"

struct UEData {
	unsigned long long imsi;
	unsigned long long msisdn;
	unsigned long long autn_num;
	unsigned long long rand_num;
	unsigned long long autn_xres;
	unsigned long long key_id;

	UEData();
	UEData(const UEData&);
	friend void swap(UEData&, UEData&);
	UEData& operator=(UEData);
	UEData(UEData&&);
	~UEData();
};

struct HSS {
	Server hss_server;
	UEData ue_data;
	MySql db_client;
	MYSQL_ROW query_res;
	string query;
	string res_row;
	time_t curr_time;
	tm *local_time;
	int curr_sec;
	int num_fields;

	HSS();
	HSS(const HSS&);
	friend void swap(HSS&, HSS&);
	HSS& operator=(HSS);
	HSS(HSS&&);
	
	void startup_hss_server(ClientDetails&);
	void setup_db_client();
	void handle_db_error();
	void recv_req_from_mme();
	void set_key_id();
	void set_autn_tokens();
	void set_autn_xres();
	void send_res_to_mme();
	~HSS();
};

#endif //HSS_H