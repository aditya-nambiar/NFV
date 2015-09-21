#include "sgwc.h"

unordered_map<int, int> g_bearer_table;
unordered_map<uint16_t, TunCdata> g_tun_ctable;

TunCdata::TunCdata() {


}

TunCdata::TunCdata(const TunCdata &src_obj) {

	mme_cteid = src_obj.mme_cteid;
	pgw_cteid = src_obj.pgw_cteid;
	pgw_port = src_obj.pgw_port;
	pgw_addr = src_obj.pgw_addr;
}

void swap(TunCdata &src_obj, TunCdata &dst_obj) {
	using std::swap;

	swap(src_obj.mme_cteid, dst_obj.mme_cteid);
	swap(src_obj.pgw_cteid, dst_obj.pgw_cteid);
	swap(src_obj.pgw_port, dst_obj.pgw_port);
	swap(src_obj.pgw_addr, dst_obj.pgw_addr);
}

TunCdata& TunCdata::operator=(TunCdata src_obj) {

	swap(*this, src_obj);
	return *this;
}

TunCdata::TunCdata(TunCdata &&src_obj)
	:TunCdata() {

	swap(*this, src_obj);
}

TunCdata::~TunCdata() {


}

SGWc::SGWc() {

	type = 1;
	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	reply = allocate_str_mem(BUFFER_SIZE);
}

SGWc::SGWc(const SGWc &src_obj) {

	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	reply = allocate_str_mem(BUFFER_SIZE);
	to_pgw = src_obj.to_pgw;
	pkt = src_obj.pkt;
	ue_num = src_obj.ue_num;
	cteid = src_obj.cteid;
	bearer_id = src_obj.bearer_id;
	type = src_obj.type;
	strcpy(ue_ip, src_obj.ue_ip);
	strcpy(reply, src_obj.ue_ip);
	tun_cdata = src_obj.tun_cdata;
}

void swap(SGWc &src_obj, SGWc &dst_obj) {
	using std::swap;

	swap(src_obj.to_pgw, dst_obj.to_pgw);
	swap(src_obj.pkt, dst_obj.pkt);
	swap(src_obj.ue_num, dst_obj.ue_num);
	swap(src_obj.cteid, dst_obj.cteid);
	swap(src_obj.bearer_id, dst_obj.bearer_id);
	swap(src_obj.type, dst_obj.type);
	swap(src_obj.ue_ip, dst_obj.ue_ip);
	swap(src_obj.reply, dst_obj.reply);
	swap(src_obj.tun_cdata, dst_obj.tun_cdata);
}

SGWc& SGWc::operator=(SGWc src_obj) {

	swap(*this, src_obj);
	return *this;
}

SGWc::SGWc(SGWc&& src_obj)
	:SGWc() {

	swap(*this, src_obj);
}

void SGWc::create_session_req_from_mme(Server &sgw_server) {

	copy_data(sgw_server.pkt);
	set_ue_num();
	set_bearer_id();
	set_cteid();
	cout << "Create session request has been received at SGW for UE - " << ue_num << endl;
}

void SGWc::copy_data(Packet &arg) {

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

void SGWc::set_ue_num() {

	memcpy(&ue_num, pkt.data + sizeof(int), sizeof(int));
}

void SGWc::set_bearer_id() {

	memcpy(&bearer_id, pkt.data + 2*sizeof(int), sizeof(int));
	fill_bearer_table();
}

void SGWc::fill_bearer_table() {

	g_bearer_table[ue_num] = bearer_id;
}

void SGWc::set_cteid() {

	cteid = generate_cteid(ue_num);
}

uint16_t SGWc::generate_cteid(int &ue_number) {

	return ue_number;
}

void SGWc::create_session_req_to_pgw(uint16_t &uteid) {

	connect_with_pgw();
	to_pgw.pkt.clear_data();
	to_pgw.pkt.fill_data(0, sizeof(int), ue_num);
	to_pgw.pkt.fill_data(sizeof(int), sizeof(int), bearer_id);
	to_pgw.pkt.fill_data(2 * sizeof(int), sizeof(uint16_t), cteid);
	to_pgw.pkt.fill_data(2 * sizeof(int) + sizeof(uint16_t), sizeof(uint16_t), uteid);
	to_pgw.pkt.make_data_packet();
	to_pgw.write_data();
}

void SGWc::connect_with_pgw() {

	to_pgw.bind_client();
	to_pgw.fill_server_details(g_pgw_port, g_pgw_addr);
	cout << "Connecting with PGW..." << endl;
	to_pgw.connect_with_server(ue_num);
	handshake_with_pgw();
	cout << "Connected with PGW" << endl;
}

void SGWc::handshake_with_pgw() {

	to_pgw.pkt.clear_data();
	to_pgw.pkt.fill_data(0, sizeof(int), type);
	to_pgw.pkt.make_data_packet();
	to_pgw.write_data();
}

void SGWc::create_session_res_from_pgw(uint16_t &pgw_uteid) {

	cout << "Waiting to read Create session response from PGW for UE - " << ue_num << endl;
	to_pgw.read_data();
	to_pgw.pkt.rem_gtpc_hdr();
	set_tun_cdata();
	memcpy(&pgw_uteid, to_pgw.pkt.data + sizeof(uint16_t), sizeof(uint16_t));
	set_ue_ip();	
}

void SGWc::set_tun_cdata() {

	memcpy(&tun_cdata.mme_cteid, pkt.data + 3*sizeof(int), sizeof(uint16_t));
	memcpy(&tun_cdata.pgw_cteid, to_pgw.pkt.data, sizeof(uint16_t));
	tun_cdata.pgw_port = g_pgw_port;
	tun_cdata.pgw_addr.assign(g_pgw_addr);
}

void SGWc::set_ue_ip() {

	memcpy(ue_ip, to_pgw.pkt.data + 2*sizeof(uint16_t), INET_ADDRSTRLEN);		
}

void SGWc::create_session_res_to_mme(Server &sgw_server) {

	strcpy(reply, "OK");
	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, sizeof(uint16_t), cteid);
	sgw_server.pkt.fill_data(sizeof(uint16_t), strlen(reply), reply);
	sgw_server.pkt.fill_gtpc_hdr(tun_cdata.mme_cteid);
	sgw_server.pkt.add_gtpc_hdr();
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
	cout << "Create session response has been sent to MME for UE - " << ue_num << endl;
}

void SGWc::modify_session_req_from_mme(Server &sgw_server, uint16_t &enodeb_uteid) {

	sgw_server.read_data();
	sgw_server.pkt.rem_gtpc_hdr();
	copy_data(sgw_server.pkt);
	memcpy(&enodeb_uteid, pkt.data, sizeof(uint16_t));
	cout << "Modify session request has been received at SGW for UE - " << ue_num << endl;
}

void SGWc::modify_session_res_to_mme(Server &sgw_server, uint16_t &sgw_uteid) {

	strcpy(reply, "OK");
	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, sizeof(uint16_t), sgw_uteid);
	sgw_server.pkt.fill_data(sizeof(uint16_t), INET_ADDRSTRLEN, ue_ip);
	sgw_server.pkt.fill_data(sizeof(uint16_t) + INET_ADDRSTRLEN, strlen(reply), reply);
	sgw_server.pkt.fill_gtpc_hdr(tun_cdata.mme_cteid);
	sgw_server.pkt.add_gtpc_hdr();
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
	cout << "Modify session response has been sent to MME for UE - " << ue_num << endl;
}

void SGWc::fill_pgw_addr(int &pgw_port, string &pgw_addr) {

	pgw_port = tun_cdata.pgw_port;
	pgw_addr = tun_cdata.pgw_addr;
}

void SGWc::fill_tun_ctable() {

	g_tun_ctable[cteid] = tun_cdata;
}

void SGWc::delete_session_req_from_mme(Server &sgw_server) {
	int type;

	sgw_server.read_data();
	sgw_server.pkt.rem_gtpc_hdr();
	memcpy(&type, sgw_server.pkt.data, sizeof(int));
	copy_pkts(sgw_server.pkt, to_pgw.pkt);
	if (type == 3) {
		cout << "Detach request for UE - " << ue_num << " has been received at SGW" << endl;
	}
}

void SGWc::copy_pkts(Packet &from_pkt, Packet &to_pkt) {

	to_pkt.clear_data();
	to_pkt.fill_data(0, from_pkt.data_len, from_pkt.data);
}

void SGWc::delete_session_req_to_pgw() {

	to_pgw.pkt.fill_gtpc_hdr(tun_cdata.pgw_cteid);
	to_pgw.pkt.add_gtpc_hdr();
	to_pgw.pkt.make_data_packet();
	to_pgw.write_data();
	erase_bearer_table();
}

void SGWc::erase_bearer_table() {

	g_bearer_table.erase(ue_num);
}

void SGWc::delete_session_res_from_pgw() {

	cout << "Waiting to read Delete session response from PGW for UE - " << ue_num << endl;
	to_pgw.read_data();
	to_pgw.pkt.rem_gtpc_hdr();
	memcpy(reply, to_pgw.pkt.data, to_pgw.pkt.data_len);
	cout << "Response is " << reply << endl;
	if (strcmp((const char*)reply, "OK") == 0) {
		cout << "PGW has successfully deallocated resources for UE - " << ue_num << endl;
	}
}

void SGWc::delete_session_res_to_mme(Server &sgw_server) {

	strcpy(reply, "OK");
	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, strlen(reply), reply);
	sgw_server.pkt.fill_gtpc_hdr(tun_cdata.mme_cteid);
	sgw_server.pkt.add_gtpc_hdr();
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
	erase_tun_ctable();
	cout << "Delete session response has been sent to MME for UE - " << ue_num << endl;
}

void SGWc::erase_tun_ctable() {

	// g_tun_ctable.erase(cteid);
}

SGWc::~SGWc() {

	free(ue_ip);
	free(reply);
}
