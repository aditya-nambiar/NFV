#include "mme.h"

void Gateways::set_gateways(){
	sgw1 = SGW1; 
	sgw2 = SGW2;
	sgw3 = SGW3;
	pgw = PGW;
}

void *multithreading_func(void *arg){
	int type;
	ClientDetails ue = *(ClientDetails*)arg;
	Server mme;
	mme.fill_server_details(g_mme_port+ue.num, g_mme_address);
	mme.bind_server();
	//setsockopt(mme.server_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));	
	mme.client_sock_addr = ue.client_sock_addr;
	mme.connect_with_client();
	mme.read_data();
	Client to_hss;
	to_hss.fill_server_details(g_hss_port, g_hss_address);
	to_hss.connect_with_server(ue.num);
	memcpy(&type, mme.pkt.data, sizeof(type));
	if(type == 1){
		authenticate(mme, to_hss);
		setup_tunnel(mme, to_hss);
	}
}

void authenticate(Server &mme, Client &to_hss){
	unsigned long long xres, res;
	const char *msg;	
	to_hss.pkt.clear_data();
	to_hss.pkt.fill_data(0, mme.pkt.data_len, (const char*)mme.pkt.data);
	to_hss.pkt.add_data();
	to_hss.write_data();
	to_hss.read_data();
	memcpy(&xres, to_hss.pkt.data + 2*sizeof(unsigned long long), sizeof(xres));
	mme.pkt.clear_data();
	mme.pkt.fill_data(0, 2*sizeof(unsigned long long), (const char*)to_hss.pkt.data);
	mme.pkt.add_data();
	mme.write_data();
	mme.read_data();
	memcpy(&res, mme.pkt.data, sizeof(res));
	if(xres == res){
		msg = "OK";
		mme.pkt.clear_data();
		mme.pkt.fill_data(0, strlen(msg), msg);
		mme.pkt.add_data();
		mme.write_data();
	}
}

void setup_tunnel(Server &mme, Client &to_hss){
	mme.read_data();
	Client to_sgw;



}

int main(){
	Gateways::set_gateways();
	Server mme;
	mme.fill_server_details(g_mme_port, g_mme_address);
	mme.bind_server();
	mme.listen_accept(multithreading_func);
  return 0;
}
