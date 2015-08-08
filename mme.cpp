#include "mme.h"

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
	Server mme;
	mme.fill_server_details(g_freeport, g_mme_addr);
	mme.bind_server();
	//setsockopt(mme.server_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));	
	mme.client_sock_addr = ue.client_sock_addr;
	mme.client_num = ue.num;
	mme.connect_with_client();
	mme.read_data();
	Client to_hss;
	to_hss.bind_client();
	to_hss.fill_server_details(g_hss_port, g_hss_addr);
	to_hss.connect_with_server(ue.num);
	memcpy(&type, mme.pkt.data, sizeof(type));
	if(type == 1){
		authenticate(mme, to_hss);
		setup_tunnel(mme, to_hss, mme.client_num);
	}
}

void authenticate(Server &mme, Client &to_hss){
	unsigned long long xres, res;
	const char *msg;	
	to_hss.pkt.clear_data();
	to_hss.pkt.fill_data(0, mme.pkt.data_len, (const char*)mme.pkt.data);
	to_hss.pkt.make_data_packet();
	to_hss.write_data();
	to_hss.read_data();
	memcpy(&xres, to_hss.pkt.data + 2*sizeof(unsigned long long), sizeof(xres));
	mme.pkt.clear_data();
	mme.pkt.fill_data(0, 2*sizeof(unsigned long long), (const char*)to_hss.pkt.data);
	mme.pkt.make_data_packet();
	mme.write_data();
	mme.read_data();
	memcpy(&res, mme.pkt.data, sizeof(res));
	if(xres == res){
		msg = "OK";
		mme.pkt.clear_data();
		mme.pkt.fill_data(0, strlen(msg), msg);
		mme.pkt.make_data_packet();
		mme.write_data();
	}
}

void setup_tunnel(Server &mme, Client &to_hss, int ue_num){
	Gateways gw;
	Tunnel tun;
	char *ue_addr = allocate_str_mem(IP_MAXPACKET);
	gw.set_sgw();
	gw.set_pgw();
	tun.set_mme_cteid(ue_num);	
	mme.read_data();
	Client to_sgw;
	to_sgw.bind_client();
	to_sgw.fill_server_details(gw.sgw_port, gw.sgw_addr);
	to_sgw.connect_with_server(ue_num);
	// to_sgw.pkt.fill_ip_hdr(g_mme_addr, gw.sgw_addr);
	// to_sgw.pkt.fill_udp_addr();
	create_session(to_sgw, ue_num, tun);
	memcpy(&tun.enodeb_uteid, mme.pkt.data, sizeof(uint16_t));
	modify_session(to_sgw, ue_num, tun);
	memcpy(&tun.sgw_uteid, to_sgw.pkt.data, sizeof(uint16_t));
	memcpy(ue_addr, to_sgw.pkt.data + sizeof(uint16_t), INET_ADDRSTRLEN);
	mme.pkt.clear_data();
	mme.pkt.fill_data(0, sizeof(uint16_t), tun.sgw_uteid);	
	mme.pkt.fill_data(sizeof(uint16_t), INET_ADDRSTRLEN, ue_addr);	
	mme.pkt.make_data_packet();
	mme.write_data();
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
	to_sgw.pkt.rem_gtpu_hdr();
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
	to_sgw.pkt.add_gtpu_hdr();
	to_sgw.pkt.make_data_packet();
	to_sgw.write_data();
	to_sgw.read_data();
	to_sgw.pkt.rem_gtpu_hdr();
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

int main(){
	Server mme;
	mme.fill_server_details(g_mme_port, g_mme_addr);
	mme.bind_server();
	mme.listen_accept(multithreading_func);
  return 0;
}
