#include "mme.h"

void *multithreading_func(void *arg){
	int type;
	unsigned long long xres, res;
	const char *msg;
	ClientDetails ue = *(ClientDetails*)arg;
	Server mme();
	mme.fill_server_details(g_mme_port+ue.num, g_mme_address);
	mme.bind_server();
	//setsockopt(mme.server_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&g_timeout, sizeof(timeval));	
	mme.client_sock_addr = ue.client_sock_addr;
	mme.connect_with_client();
	mme.read_data();
	Client mme_user;
	mme_user.fill_server_details(g_hss_port, g_hss_address);
	mme_user.connect_with_server(ue.num);
	memcpy(&type, mme.server_buffer, sizeof(type));
	if(type == 1){
		bzero(mme_user.client_buffer, BUFFER_SIZE);
		memcpy(mme_user.client_buffer, mme.server_buffer, BUFFER_SIZE-1);
		mme_user.write_data();
		mme_user.read_data();
		memcpy(&xres, mme_user.client_buffer+2*sizeof(unsigned long long), sizeof(xres));
		bzero(mme.server_buffer, BUFFER_SIZE);
		memcpy(mme.server_buffer, mme_user.client_buffer, 2*sizeof(unsigned long long));
		mme.write_data();
		mme.read_data();
		memcpy(&res, mme.server_buffer, sizeof(res));
		if(xres == res){
			msg = "OK";
			strcpy(mme.server_buffer, msg);
			mme.write_data();
		}
	}
}

int main(){
	Server mme(g_mme_port, g_mme_address);
	mme.listen_accept(multithreading_func);
  return 0;
}
