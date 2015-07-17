#include "sgw.h"

void* multithreading_func(void *arg){
	int type;
	ClientDetails *entity = *(ClientDetails*)arg;
	Server sgw;
	sgw.fill_server_details(g_sgw1_port + entity.num, g_sgw1_address);
	sgw.bind_server();
	sgw.client_sock_addr = entity.client_sock_addr;
	sgw.connect_with_client();
	sgw.read_data();
	memcpy(&type, sgw.pkt.data, sizeof(int));
	if(type == 1)
		handle_cdata(sgw);
	else if(type == 2)
		handle_udata(sgw);
}

void handle_cdata(Server &sgw){

}

int main(){
	Server sgw;
	sgw.fill_server_details(g_sgw1_port, g_sgw1_address);
	sgw.bind_server();
	sgw.listen_accept(multithreading_func);
	return 0;
}