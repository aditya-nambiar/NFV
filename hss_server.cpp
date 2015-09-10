#include "hss_server.h"

void* process_req(void *arg){
	ClientDetails entity;
	HSS hss;

	entity = *(ClientDetails*)arg;
	hss.startup_hss_server(entity);
	hss.setup_db_client();
	hss.recv_req_from_mme();
	hss.set_key_id();
	hss.set_autn_tokens();
	hss.set_autn_xres();
	hss.send_res_to_mme();
	mysql_thread_end();
}

int main(int argc, char *argv[]){
	Server hss_server;
	
	check_server_usage(argc, argv);
	if(mysql_library_init(0, NULL, NULL))
		cout<<"ERROR: mysql library cannot be opened"<<endl;
	hss_server.begin_thread_pool(atoi(argv[1]), process_req);
	hss_server.fill_server_details(g_hss_port, g_hss_addr);
	hss_server.bind_server();
	hss_server.listen_accept();
	mysql_library_end();
	return 0;
}
