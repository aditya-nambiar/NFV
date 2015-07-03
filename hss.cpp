#include "hss.h"

const char *g_query;

void* multithreading_func(void *arg){
	int type;
	unsigned long long imsi, msisdn;
	ClientDetails mme = *(ClientDetails*)arg;
	Server hss(g_hss_port+mme.num, g_hss_address);
	//setsockopt(hss.server_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));		
	hss.client_sock_addr = mme.client_sock_addr;
	hss.connect_with_client();
	hss.read_data();
	memcpy(&type, hss.server_buffer, sizeof(type));
	if(type == 1){
		memcpy(&imsi, hss.server_buffer+sizeof(type), sizeof(imsi));
		memcpy(&msisdn, hss.server_buffer+sizeof(type)+sizeof(imsi), sizeof(msisdn));
		authenticate_query(imsi, msisdn, hss.server_buffer);
		hss.write_data();
	}		
	close(hss.server_socket);	
	mysql_thread_end();

}

void authenticate_query(unsigned long long imsi, unsigned long long msisdn, char *xres){
	int num_fields;
	MYSQL_ROW row;	
	const char *res_str;
	unsigned long long res, autn, rand, key_id;
	int time_sec;
	string inp_query = "", imsi_str, msisdn_str;
	imsi_str = to_char_array(imsi);
	msisdn_str = to_char_array(msisdn);
	MySql db;
	connect_with_db(db);
	inp_query = inp_query + "select key_id from ue_data where imsi = " + imsi_str + " and msisdn = " + msisdn_str;
	g_query = inp_query.c_str();
	db.perform_query(g_query);
	row = mysql_fetch_row(db.result);
	if(row == 0){
		cout<<"ERROR"<<endl;
		exit(EXIT_FAILURE);
	}
	res_str = row[0];	
	key_id = strtoull(res_str,NULL,10);

	time_t now = time(0);
	tm *ltm = localtime(&now);
	time_sec = ltm->tm_sec;
	inp_query = (string)"select * from ue_autn where autn like '%" + to_char_array(time_sec) + "'";
	g_query = inp_query.c_str();
	db.perform_query(g_query);
	num_fields = mysql_num_fields(db.result);
	row = mysql_fetch_row(db.result);
	if(row == 0){
		cout<<"ERROR"<<endl;
		exit(EXIT_FAILURE);
	}
	for(int i=0;i<num_fields;i++){
		res_str = row[i];
		if(i==0)
			autn = strtoull(res_str,NULL,10);
		else
			rand = strtoull(res_str,NULL,10);
	}
	res = autn*key_id + rand*(key_id+1);
	bzero(xres, BUFFER_SIZE);
	memcpy(xres, &autn, sizeof(autn));
	memcpy(xres+sizeof(autn), &rand, sizeof(rand));
	memcpy(xres+sizeof(autn)+sizeof(rand), &res, sizeof(res));
}

void connect_with_db(MySql &db){
	db.conn_setup();
}

void fetch_db_data(MySql &db, const char *query){	
	db.perform_query(query);
	db.fetch_result();
}

int main(){
	if(mysql_library_init(0, NULL, NULL))
		cout<<"ERROR: mysql library cannot be opened"<<endl;
	Server hss(g_hss_port, g_hss_address);
	hss.listen_accept(multithreading_func);
	mysql_library_end();
	return 0;
}
