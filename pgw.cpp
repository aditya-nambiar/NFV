#include "pgw.h"

void* multithreading_func(void *arg){
	int type;
	ClientDetails *entity = *(ClientDetails*)arg;
	Server pgw;
	pgw.fill_server_details(g_pgw_port + entity.num, g_pgw_address);
	pgw.bind_server();
	pgw.client_sock_addr = entity.client_sock_addr;
	pgw.connect_with_client();
	pgw.read_data();
	memcpy(&type, pgw.pkt.data, sizeof(int));
	if(type == 1)
		handle_cdata(pgw);
	else if(type == 2)
		handle_udata(pgw);
}

void handle_cdata(Server &pgw){

}

int main(){
	Server pgw;
	pgw.fill_server_details(g_pgw_port, g_pgw_address);
	pgw.bind_server();
	pgw.listen_accept(multithreading_func);
	return 0;
}