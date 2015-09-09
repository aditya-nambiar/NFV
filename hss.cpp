#include "hss.h"

UEData::UEData(){


}

UEData::UEData(const UEData &src_obj){

	imsi = src_obj.imsi;
	msisdn = src_obj.msisdn;
	autn_num = src_obj.autn_num;
	rand_num = src_obj.rand_num;
	autn_xres = src_obj.autn_xres;
	key_id = src_obj.key_id;
}

void swap(UEData &src_obj, UEData &dst_obj){
	using std::swap;

	swap(src_obj.imsi, dst_obj.imsi);
	swap(src_obj.msisdn, dst_obj.msisdn);
	swap(src_obj.autn_num, dst_obj.autn_num);
	swap(src_obj.rand_num, dst_obj.rand_num);
	swap(src_obj.autn_xres, dst_obj.autn_xres);
	swap(src_obj.key_id, dst_obj.key_id);
}

UEData& UEData::operator=(UEData src_obj){

	swap(*this, src_obj);
	return *this;
}

UEData::UEData(UEData &&src_obj)
	:UEData(){

	swap(*this, src_obj);
}

UEData::~UEData(){


}

HSS::HSS(){

}

HSS::HSS(const HSS &src_obj){

	hss_server = src_obj.hss_server;
	ue_data = src_obj.ue_data;
	db_client = src_obj.db_client;
	query_res = src_obj.query_res;
	query = src_obj.query;
	res_row = src_obj.res_row;
	curr_time = src_obj.curr_time;
	local_time = src_obj.local_time;
	curr_sec = src_obj.curr_sec;
	num_fields = src_obj.num_fields;
}

void swap(HSS &src_obj, HSS &dst_obj){
	using std::swap;

	swap(src_obj.hss_server, dst_obj.hss_server);
	swap(src_obj.ue_data, dst_obj.ue_data);
	swap(src_obj.db_client, dst_obj.db_client);
	swap(src_obj.query_res, dst_obj.query_res);
	swap(src_obj.query, dst_obj.query);
	swap(src_obj.res_row, dst_obj.res_row);
	swap(src_obj.curr_time, dst_obj.curr_time);
	swap(src_obj.local_time, dst_obj.local_time);
	swap(src_obj.curr_sec, dst_obj.curr_sec);
	swap(src_obj.num_fields, dst_obj.num_fields);
}

HSS& HSS::operator=(HSS src_obj){

	swap(*this, src_obj);
	return *this;
}

HSS::HSS(HSS &&src_obj)
	:HSS(){

	swap(*this, src_obj);
}

void HSS::startup_hss_server(ClientDetails &entity){

	hss_server.fill_server_details(g_freeport, g_hss_addr);
	hss_server.bind_server();
	hss_server.client_sock_addr = entity.client_sock_addr;
	hss_server.client_num = entity.num;
	hss_server.connect_with_client();
}

void HSS::setup_db_client(){

	MySql::set_conn_details();
	db_client.setup_conn();	
}

void HSS::handle_db_error(){

	if(query_res == 0){
		cout<<"ERROR: No rows fetched for this query - ";
		cout<<query<<endl;
		exit(EXIT_FAILURE);
	}
}

void HSS::recv_req_from_mme(){

	hss_server.read_data();
	memcpy(&ue_data.imsi, hss_server.pkt.data, sizeof(unsigned long long));
	memcpy(&ue_data.msisdn, hss_server.pkt.data + sizeof(unsigned long long), sizeof(unsigned long long));	
}

void HSS::set_key_id(){

	query = "select key_id from ue_data where imsi = " + to_string(ue_data.imsi) + " and msisdn = " + to_string(ue_data.msisdn);
	db_client.perform_query(query.c_str());
	query_res = mysql_fetch_row(db_client.result);
	handle_db_error();
	res_row = query_res[0];	
	ue_data.key_id = stoull(res_row);
}

void HSS::set_autn_tokens(){
	int i;

	curr_time = time(0);
	local_time = localtime(&curr_time);
	curr_sec = local_time->tm_sec;
	query = "select * from ue_autn where autn like '%" + to_string(curr_sec) + "'";
	db_client.perform_query(query.c_str());
	num_fields = mysql_num_fields(db_client.result);
	query_res = mysql_fetch_row(db_client.result);
	handle_db_error();
	for(i=0; i<num_fields; i++){
		res_row = query_res[i];
		if(i == 0){
			ue_data.autn_num = stoull(res_row);
		}
		else{
			ue_data.rand_num = stoull(res_row);
		}
	}
}

void HSS::set_autn_xres(){

	ue_data.autn_xres = (ue_data.autn_num * ue_data.key_id) + (ue_data.rand_num * (ue_data.key_id + 1));
}

void HSS::send_res_to_mme(){

	hss_server.pkt.clear_data();
	hss_server.pkt.fill_data(0, sizeof(unsigned long long), ue_data.autn_num);
	hss_server.pkt.fill_data(sizeof(unsigned long long), sizeof(unsigned long long), ue_data.rand_num);
	hss_server.pkt.fill_data(2 * sizeof(unsigned long long), sizeof(unsigned long long), ue_data.autn_xres);
	hss_server.pkt.make_data_packet();
	hss_server.write_data();	
}

HSS::~HSS(){


}