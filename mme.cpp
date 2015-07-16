#include "mme.h"

void *multithreading_func(void *arg){
	int type;
	unsigned long long xres, res;
	const char *msg;
	ClientDetails ue = *(ClientDetails*)arg;
	Server mme;
	mme.fill_server_details(g_mme_port+ue.num, g_mme_address);
	mme.bind_server();
	//setsockopt(mme.server_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));	
	mme.client_sock_addr = ue.client_sock_addr;
	mme.connect_with_client();
	mme.read_data();
	Client mme_user;
	mme_user.fill_server_details(g_hss_port, g_hss_address);
	mme_user.connect_with_server(ue.num);
	memcpy(&type, mme.pkt.data, sizeof(type));
	if(type == 1){
		mme_user.pkt.clear_data();
		mme_user.pkt.fill_data(0, mme.pkt.data_len, (const char*)mme.pkt.data);
		mme_user.pkt.add_data();
		mme_user.write_data();
		mme_user.read_data();
		memcpy(&xres, mme_user.pkt.data + 2*sizeof(unsigned long long), sizeof(xres));
		mme.pkt.clear_data();
		mme.pkt.fill_data(0, 2*sizeof(unsigned long long), (const char*)mme_user.pkt.data);
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
}

int main(){
	Server mme;
	mme.fill_server_details(g_mme_port, g_mme_address);
	mme.bind_server();
	mme.listen_accept(multithreading_func);
  return 0;
}
