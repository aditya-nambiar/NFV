#include "mme.h"

TunData::TunData() {

	sgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
	pgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

TunData::TunData(const TunData &src_obj) {

	sgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
	pgw_addr = allocate_str_mem(INET_ADDRSTRLEN);	
	mme_cteid = src_obj.mme_cteid;
	sgw_cteid = src_obj.sgw_cteid;
	sgw_uteid = src_obj.sgw_uteid;
	enodeb_uteid = src_obj.enodeb_uteid;
	sgw_port = src_obj.sgw_port;
	pgw_port = src_obj.pgw_port;
	strcpy(sgw_addr, src_obj.sgw_addr);
	strcpy(pgw_addr, src_obj.pgw_addr);
}

void swap(TunData &src_obj, TunData &dst_obj) {
	using std::swap;

	swap(src_obj.mme_cteid, dst_obj.mme_cteid);
	swap(src_obj.sgw_cteid, dst_obj.sgw_cteid);
	swap(src_obj.sgw_uteid, dst_obj.sgw_uteid);
	swap(src_obj.enodeb_uteid, dst_obj.enodeb_uteid);
	swap(src_obj.sgw_port, dst_obj.sgw_port);
	swap(src_obj.pgw_port, dst_obj.pgw_port);
	swap(src_obj.sgw_addr, dst_obj.sgw_addr);
	swap(src_obj.pgw_addr, dst_obj.pgw_addr);
}

TunData& TunData::operator=(TunData src_obj) {

	swap(*this, src_obj);
	return *this;
}

TunData::TunData(TunData &&src_obj)
	:TunData() {

	swap(*this, src_obj);
}

TunData::~TunData() {

	free(sgw_addr);
	free(pgw_addr);
}

MME::MME() {

	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	reply = allocate_str_mem(BUFFER_SIZE);
}

MME::MME(const MME &src_obj) {

	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	reply = allocate_str_mem(BUFFER_SIZE);
	mme_server = src_obj.mme_server;
	to_hss = src_obj.to_hss;
	to_sgw = src_obj.to_sgw;
	ue_num = src_obj.ue_num;
	bearer_id = src_obj.bearer_id;
	type = src_obj.type;
	strcpy(ue_ip, src_obj.ue_ip);
	strcpy(reply, src_obj.reply);
	autn_num = src_obj.autn_num;
	rand_num = src_obj.rand_num;
	autn_xres = src_obj.autn_xres;
	autn_res = src_obj.autn_res;
	tun_data = src_obj.tun_data;
}

void swap(MME &src_obj, MME &dst_obj) {
	using std::swap;

	swap(src_obj.mme_server, dst_obj.mme_server);
	swap(src_obj.to_hss, dst_obj.to_hss);
	swap(src_obj.to_sgw, dst_obj.to_sgw);
	swap(src_obj.ue_num, dst_obj.ue_num);
	swap(src_obj.bearer_id, dst_obj.bearer_id);
	swap(src_obj.type, dst_obj.type);
	swap(src_obj.ue_ip, dst_obj.ue_ip);
	swap(src_obj.reply, dst_obj.reply);
	swap(src_obj.autn_num, dst_obj.autn_num);
	swap(src_obj.rand_num, dst_obj.rand_num);
	swap(src_obj.autn_xres, dst_obj.autn_xres);
	swap(src_obj.autn_res, dst_obj.autn_res);
	swap(src_obj.tun_data, dst_obj.tun_data);
}

MME& MME::operator=(MME src_obj) {

	swap(*this, src_obj);
	return *this;
}

MME::MME(MME &&src_obj)
	:MME() {

	swap(*this, src_obj);
}

void MME::set_cteid() {

	tun_data.mme_cteid = generate_cteid(ue_num);
}

uint16_t MME::generate_cteid(int &ue_number) {

	return ue_number;
}

void MME::set_bearer_id() {

	bearer_id = generate_bearer_id(ue_num);
}

int MME::generate_bearer_id(int &ue_number) {

	return ue_number;
}

void MME::set_sgw() {
	
	tun_data.sgw_port = g_sgw1_port;
	strcpy(tun_data.sgw_addr, g_sgw1_addr);
}

void MME::set_pgw() {

	tun_data.pgw_port = g_pgw_port;
	strcpy(tun_data.pgw_addr, g_pgw_addr);
}

void MME::startup_mme_server(ClientDetails &entity) {

	mme_server.fill_server_details(g_freeport, g_mme_addr);
	mme_server.bind_server();
	mme_server.client_sock_addr = entity.client_sock_addr;
	mme_server.client_num = entity.num;
	mme_server.connect_with_client();
}

void MME::set_ue_num() {

	ue_num = mme_server.client_num;
}

void MME::attach_req_from_ue() {

	mme_server.read_data();
}

void MME::setup_hss_client() {

	to_hss.bind_client();
	to_hss.fill_server_details(g_hss_port, g_hss_addr);
	to_hss.connect_with_server(ue_num);
}

void MME::fetch_ue_data() {

	to_hss.pkt.clear_data();
	to_hss.pkt.fill_data(0, mme_server.pkt.data_len, mme_server.pkt.data);
	to_hss.pkt.make_data_packet();
	to_hss.write_data();
	to_hss.read_data();	
	memcpy(&autn_num, to_hss.pkt.data, sizeof(unsigned long long));
	memcpy(&rand_num, to_hss.pkt.data + sizeof(unsigned long long), sizeof(unsigned long long));
	memcpy(&autn_xres, to_hss.pkt.data + 2 * sizeof(unsigned long long), sizeof(unsigned long long));
}

void MME::authenticate_ue() {

	mme_server.pkt.clear_data();
	mme_server.pkt.fill_data(0, 2 * sizeof(unsigned long long), to_hss.pkt.data);
	mme_server.pkt.make_data_packet();
	mme_server.write_data();
	mme_server.read_data();
	memcpy(&autn_res, mme_server.pkt.data, sizeof(unsigned long long));
	if (autn_xres == autn_res) {
		strcpy(reply, "OK");
		mme_server.pkt.clear_data();
		mme_server.pkt.fill_data(0, strlen(reply), reply);
		mme_server.pkt.make_data_packet();
		mme_server.write_data();
		cout << "Authentication is successful for UE - " << ue_num << endl;
	}	
	else {
		strcpy(reply, "FAILED");
		mme_server.pkt.clear_data();
		mme_server.pkt.fill_data(0, strlen(reply), reply);
		mme_server.pkt.make_data_packet();
		mme_server.write_data();
		cout << "Authentication is not successful for UE - " << ue_num << endl;
	}
}

void MME::setup_sgw_client() {

	to_sgw.bind_client();
	to_sgw.fill_server_details(tun_data.sgw_port, tun_data.sgw_addr);
	to_sgw.connect_with_server(ue_num);
}

void MME::create_session_req_to_sgw() {

	type = 1;
	to_sgw.pkt.clear_data();
	to_sgw.pkt.fill_data(0, sizeof(int), type);
	to_sgw.pkt.fill_data(sizeof(int), sizeof(int), ue_num);
	to_sgw.pkt.fill_data(2 * sizeof(int), sizeof(int), bearer_id);
	to_sgw.pkt.fill_data(3 * sizeof(int), sizeof(uint16_t), tun_data.mme_cteid);
	to_sgw.pkt.make_data_packet();
	to_sgw.write_data();
}

void MME::create_session_res_from_sgw() {

	cout << "Waiting to read Create Session Request response from SGW" << endl;
	to_sgw.read_data();
	to_sgw.pkt.rem_gtpc_hdr();
	memcpy(&tun_data.sgw_cteid, to_sgw.pkt.data, sizeof(uint16_t));
	memcpy(reply, to_sgw.pkt.data + sizeof(uint16_t), to_sgw.pkt.data_len - sizeof(uint16_t));
	if (strcmp((const char*)reply, "OK") == 0) {
		cout << "Create session request was successful for UE - " << ue_num << endl;
	}
	else {
		cout << "Create session request failed: Please disconnect and connect again" << endl;
		handle_exceptions();
	}
}

void MME::recv_enodeb() {

	mme_server.read_data();	
	memcpy(&tun_data.enodeb_uteid, mme_server.pkt.data, sizeof(uint16_t));
}

void MME::modify_session_req_to_sgw() {

	to_sgw.pkt.clear_data();
	to_sgw.pkt.fill_gtpc_hdr(tun_data.sgw_cteid);
	to_sgw.pkt.fill_data(0, sizeof(uint16_t), tun_data.enodeb_uteid);
	to_sgw.pkt.add_gtpc_hdr();
	to_sgw.pkt.make_data_packet();
	to_sgw.write_data();	
}

void MME::modify_session_res_from_sgw() {

	to_sgw.read_data();
	to_sgw.pkt.rem_gtpc_hdr();
	memcpy(&tun_data.sgw_uteid, to_sgw.pkt.data, sizeof(uint16_t));
	memcpy(ue_ip, to_sgw.pkt.data + sizeof(uint16_t), INET_ADDRSTRLEN);
	memcpy(reply, to_sgw.pkt.data + sizeof(uint16_t) + INET_ADDRSTRLEN, to_sgw.pkt.data_len - sizeof(uint16_t) - INET_ADDRSTRLEN);
	if (strcmp((const char*)reply, "OK") == 0) {
		cout << "Modify Session Request was successful for UE - " << ue_num << endl;
	}
	else {
		cout << "Modify session request failed: Please disconnect and connect again" << endl;
		handle_exceptions();		
	}
}

void MME::send_enodeb() {

	mme_server.pkt.clear_data();
	mme_server.pkt.fill_data(0, sizeof(uint16_t), tun_data.sgw_uteid);	
	mme_server.pkt.fill_data(sizeof(uint16_t), INET_ADDRSTRLEN, ue_ip);	
	mme_server.pkt.make_data_packet();
	mme_server.write_data();
}

void MME::detach_req_from_ue() {

	mme_server.read_data();
	memcpy(&type, mme_server.pkt.data, sizeof(int));	
	if (type == 3) {
		cout << "Detach request has been received successfully at MME for UE - " << ue_num << endl;
	}
	else {
		cout << "Invalid Detach type num - " << type << " : Please disconnect and connect again" << endl;
		handle_exceptions();
	}
}

void MME::delete_session_req_to_sgw() {

	to_sgw.pkt.clear_data();
	to_sgw.pkt.fill_data(0, mme_server.pkt.data_len, mme_server.pkt.data);
	to_sgw.pkt.fill_gtpc_hdr(tun_data.sgw_cteid);
	to_sgw.pkt.add_gtpc_hdr();
	to_sgw.pkt.make_data_packet();
	to_sgw.write_data();
}

void MME::delete_session_res_from_sgw() {

	cout << "Waiting to read Detach session response from SGW" << endl;
	to_sgw.read_data();
	to_sgw.pkt.rem_gtpc_hdr();
	memcpy(reply, to_sgw.pkt.data, to_sgw.pkt.data_len);
	if (strcmp((const char*)reply, "OK") == 0) {
		cout << "MME has received successful detach response for UE - " << ue_num << endl;
	}
	else {
		cout << "Detach process failure at SGW: UE - " << ue_num << ", Reply = " << reply << ". Please disconnect and connect again" << endl;
		handle_exceptions();
	}
}

void MME::detach_res_to_ue() {

	strcpy(reply, "OK");
	mme_server.pkt.clear_data();
	mme_server.pkt.fill_data(0, strlen(reply), reply);
	mme_server.pkt.make_data_packet();
	mme_server.write_data();
	cout << "MME has successfully deallocated resources for UE - " << ue_num << endl;
}

void MME::rem_bearer_id() {

	bearer_id = -1; // Dummy statement
}

void MME::rem_tun_data() {

	/*	Since this MME object is dedicated to this paricular UE, the moment it completes its job and goes out of scope,
		all data related to the UE will automatically de deleted
	*/
}

MME::~MME() {

	free(ue_ip);
	free(reply);
}
