#include "mme.h"

void *multithreading_func(void *arg){
	int type;
	unsigned long long xres, res;
	const char *msg;
	Server mme(g_mme_port, g_mme_address);
	mme.server_socket = *(int*)arg;
	Client mme_user;
	mme_user.input_server_details(g_hss_port, g_hss_address);
	mme_user.connect_with_server();
	mme.read_data();
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
	close(mme.server_socket);
}

int main(){
	Server mme(g_mme_port, g_mme_address);
	mme.start_listening();
	mme.accept_connections(multithreading_func);
  return 0;
}
