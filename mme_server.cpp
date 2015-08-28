#include "mme_server.h"

unordered_map<int, int> g_bearer_table;
double g_req_duration;
time_t g_start_time;

Gateways::Gateways(){
	sgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
	pgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

void Gateways::set_sgw(){
	sgw_port = g_sgw1_port;
	strcpy(sgw_addr, g_sgw1_addr);
}

void Gateways::set_pgw(){
	pgw_port = g_pgw_port;
	strcpy(pgw_addr, g_pgw_addr);
}

Gateways::~Gateways(){
	free(sgw_addr);
	free(pgw_addr);
}

void Tunnel::set_mme_cteid(int ue_num){
	mme_cteid = ue_num;
}

void *multithreading_func(void *arg){
	int type;
	
	ClientDetails ue = *(ClientDetails*)arg;
	Server mme_server;
	mme_server.fill_server_details(g_freeport, g_mme_addr);
	mme_server.bind_server();
	//setsockopt(mme_server.server_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));	
	mme_server.client_sock_addr = ue.client_sock_addr;
	mme_server.client_num = ue.num;
	mme_server.connect_with_client();
	mme_server.read_data();
	Client to_hss;
	Client to_sgw;
	Tunnel tun;
	to_hss.bind_client();
	to_hss.fill_server_details(g_hss_port, g_hss_addr);
	to_hss.connect_with_server(ue.num);
	memcpy(&type, mme_server.pkt.data, sizeof(type));
	if(type == 1){
		authenticate(mme_server, to_hss);
		setup_tunnel(mme_server, to_sgw, mme_server.client_num, tun);
		detach(mme_server, to_sgw, tun, mme_server.client_num);
	}
	// time_check(g_start_time, g_req_duration);
}

void authenticate(Server &mme_server, Client &to_hss){
	unsigned long long xres, res;
	const char *msg;	
	to_hss.pkt.clear_data();
	to_hss.pkt.fill_data(0, mme_server.pkt.data_len, (const char*)mme_server.pkt.data);
	to_hss.pkt.make_data_packet();
	to_hss.write_data();
	to_hss.read_data();
	memcpy(&xres, to_hss.pkt.data + 2*sizeof(unsigned long long), sizeof(xres));
	mme_server.pkt.clear_data();
	mme_server.pkt.fill_data(0, 2*sizeof(unsigned long long), (const char*)to_hss.pkt.data);
	mme_server.pkt.make_data_packet();
	mme_server.write_data();
	mme_server.read_data();
	memcpy(&res, mme_server.pkt.data, sizeof(res));
	if(xres == res){
		msg = "OK";
		mme_server.pkt.clear_data();
		mme_server.pkt.fill_data(0, strlen(msg), msg);
		mme_server.pkt.make_data_packet();
		mme_server.write_data();
	}
}

void setup_tunnel(Server &mme_server, Client &to_sgw, int &ue_num, Tunnel &tun){
	Gateways gw;
	char *ue_addr = allocate_str_mem(IP_MAXPACKET);
	gw.set_sgw();
	gw.set_pgw();
	tun.set_mme_cteid(ue_num);	
	mme_server.read_data();
	to_sgw.bind_client();
	to_sgw.fill_server_details(gw.sgw_port, gw.sgw_addr);
	to_sgw.connect_with_server(ue_num);
	// to_sgw.pkt.fill_ip_hdr(g_mme_addr, gw.sgw_addr);
	// to_sgw.pkt.fill_udp_addr();
	create_session(to_sgw, ue_num, tun);
	memcpy(&tun.enodeb_uteid, mme_server.pkt.data, sizeof(uint16_t));
	modify_session(to_sgw, ue_num, tun);
	memcpy(&tun.sgw_uteid, to_sgw.pkt.data, sizeof(uint16_t));
	memcpy(ue_addr, to_sgw.pkt.data + sizeof(uint16_t), INET_ADDRSTRLEN);
	cout<<"UE addr is "<<ue_addr<<endl;
	mme_server.pkt.clear_data();
	mme_server.pkt.fill_data(0, sizeof(uint16_t), tun.sgw_uteid);	
	mme_server.pkt.fill_data(sizeof(uint16_t), INET_ADDRSTRLEN, ue_addr);	
	mme_server.pkt.make_data_packet();
	mme_server.write_data();
	cout<<"Tunnel Setup successful for client - "<<ue_num<<endl;
	free(ue_addr);
}

void create_session(Client &to_sgw, int ue_num, Tunnel &tun){
	int type = 1;
	char *reply = allocate_str_mem(BUFFER_SIZE);
	set_bearer_id(ue_num);
	to_sgw.pkt.clear_data();
	to_sgw.pkt.fill_data(0, sizeof(int), type);
	to_sgw.pkt.fill_data(sizeof(int), sizeof(int), ue_num);
	to_sgw.pkt.fill_data(2 * sizeof(int), sizeof(int), g_bearer_table[ue_num]);
	to_sgw.pkt.fill_data(3 * sizeof(int), sizeof(uint16_t), tun.mme_cteid);
	to_sgw.pkt.make_data_packet();
	to_sgw.write_data();
	to_sgw.read_data();
	to_sgw.pkt.rem_gtpc_hdr();
	memcpy(&tun.sgw_cteid, to_sgw.pkt.data, sizeof(uint16_t));
	memcpy(reply, to_sgw.pkt.data + sizeof(uint16_t), to_sgw.pkt.data_len - sizeof(uint16_t));
	if(strcmp((const char*)reply, "OK") == 0){
		cout<<"Create Session Request is successful for this client - "<<ue_num<<endl;
	}
	free(reply);
}

void modify_session(Client &to_sgw, int ue_num, Tunnel &tun){
	char *reply;
	reply = allocate_str_mem(BUFFER_SIZE);
	to_sgw.pkt.clear_data();
	to_sgw.pkt.fill_gtpc_hdr(tun.sgw_cteid);
	to_sgw.pkt.fill_data(0, sizeof(uint16_t), tun.enodeb_uteid);
	to_sgw.pkt.add_gtpc_hdr();
	to_sgw.pkt.make_data_packet();
	to_sgw.write_data();
	to_sgw.read_data();
	to_sgw.pkt.rem_gtpc_hdr();
	memcpy(reply, to_sgw.pkt.data + sizeof(uint16_t) + INET_ADDRSTRLEN, to_sgw.pkt.data_len - sizeof(uint16_t) - INET_ADDRSTRLEN);
	if(strcmp((const char*)reply, "OK") == 0){
		cout<<"Modify Session Request is successful for this client - "<<ue_num<<endl;
	}
	free(reply);
}

void set_bearer_id(int ue_num){
	g_bearer_table[ue_num] = generate_bearer_id(ue_num);
}

int generate_bearer_id(int ue_num){
	return ue_num;
}

void detach(Server &mme_server, Client &to_sgw, Tunnel &tun, int &ue_num){

	detach_req_from_ran(mme_server, ue_num);
	delete_session_req_to_sgw(mme_server, to_sgw, tun);
	delete_bearer_id(ue_num);
	delete_session_res_from_sgw(mme_server, to_sgw, ue_num);
	detach_res(mme_server, ue_num);
}

void detach_req_from_ran(Server &mme_server, int &ue_num){
	int type;

	mme_server.read_data();
	memcpy(&type, mme_server.pkt.data, sizeof(int));	
	if(type == 3){
		cout<<"Detach request has been received successfully at MME for UE - "<<ue_num<<endl;
	}
}

void delete_session_req_to_sgw(Server &mme_server, Client &to_sgw, Tunnel &tun){

	to_sgw.pkt.clear_data();
	to_sgw.pkt.fill_data(0, mme_server.pkt.data_len, mme_server.pkt.data);
	to_sgw.pkt.fill_gtpc_hdr(tun.mme_cteid);
	to_sgw.pkt.add_gtpc_hdr();
	to_sgw.pkt.make_data_packet();
	to_sgw.write_data();
}

void delete_bearer_id(int &ue_num){

	g_bearer_table.erase(ue_num);
}

void delete_session_res_from_sgw(Server &mme_server, Client &to_sgw, int &ue_num){
	char *reply;

	reply = allocate_str_mem(BUFFER_SIZE);
	to_sgw.read_data();
	to_sgw.pkt.rem_gtpc_hdr();
	memcpy(reply, to_sgw.pkt.data, to_sgw.pkt.data_len);
	cout<<"Reply is "<<reply<<endl;
	if(strcmp((const char*)reply, "OK") == 0){
		cout<<"SGW has successfully deallocated resources for this UE - "<<ue_num<<endl;
	}
	free(reply);
}

void detach_res(Server &mme_server, int &ue_num){
	const char *reply;

	reply = "OK";
	mme_server.pkt.clear_data();
	mme_server.pkt.fill_data(0, strlen(reply), reply);
	mme_server.pkt.make_data_packet();
	mme_server.write_data();
	cout<<"MME has successfully deallocated resources for this UE - "<<ue_num<<endl;
}

void startup_mme(char *argv[]){

	g_start_time = time(0);
	// g_req_duration = atof(argv[2]);
}

int main(int argc, char *argv[]){
	Server mme_server;

	usage(argc, argv);
	startup_mme(argv);
	mme_server.begin_thread_pool(atoi(argv[1]), multithreading_func);
	mme_server.fill_server_details(g_mme_port, g_mme_addr);
	mme_server.bind_server();
	mme_server.listen_accept();
  return 0;
}
