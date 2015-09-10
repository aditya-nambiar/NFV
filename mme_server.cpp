#include "mme_server.h"

void* process_req(void *arg) {
	ClientDetails entity;
	MME mme;

	entity = *((ClientDetails*)arg);
	mme.startup_mme_server(entity);
	mme.set_ue_num();
	attach_ue(mme);
	detach_ue(mme);
}

void attach_ue(MME &mme) {

	recv_req_from_ue(mme);
	authenticate_ue(mme);
	setup_tunnel(mme);
}

void recv_req_from_ue(MME &mme) {

	mme.attach_req_from_ue();	
}

void authenticate_ue(MME &mme) {

	mme.setup_hss_client();
	mme.fetch_ue_data();
	mme.authenticate_ue();
}

void setup_tunnel(MME &mme) {

	mme.set_cteid();
	mme.set_sgw();
	mme.set_pgw();
	mme.set_bearer_id();
	mme.setup_sgw_client();
	mme.create_session_req_to_sgw();
	mme.create_session_res_from_sgw();
	mme.recv_enodeb();
	mme.modify_session_req_to_sgw();
	mme.modify_session_res_from_sgw();
	mme.send_enodeb();
}

void detach_ue(MME &mme) {

	mme.detach_req_from_ue();
	mme.delete_session_req_to_sgw();
	mme.rem_bearer_id();
	mme.delete_session_res_from_sgw();
	mme.rem_tun_data();
	mme.detach_res_to_ue();
}

int main(int argc, char *argv[]) {
	Server mme_server;

	check_server_usage(argc, argv);
	mme_server.begin_thread_pool(atoi(argv[1]), process_req);
	mme_server.fill_server_details(g_mme_port, g_mme_addr);
	mme_server.bind_server();
	mme_server.listen_accept();
	return 0;
}
