#include "sgw_server.h"

void* process_traffic(void *arg){
	int type;
	ClientDetails entity = *(ClientDetails*)arg;
	Server sgw_server;

	sgw_server.fill_server_details(g_freeport, g_sgw1_addr);
	sgw_server.bind_server();
	sgw_server.client_sock_addr = entity.client_sock_addr;
	sgw_server.client_num = entity.num;
	sgw_server.connect_with_client();
	sgw_server.read_data();
	memcpy(&type, sgw_server.pkt.data, sizeof(int));
	if(type == 1){
		handle_cdata(sgw_server);
	}
	if(type == 2){
		handle_udata(sgw_server);
	}
	// time_check(g_start_time, g_req_duration);
}

void handle_cdata(Server &sgw_server){
	SGWc sgwc;
	SGWu sgwu;
	TunUdata tun_udata;
	uint16_t uteid;

	sgwc.create_session_req_from_mme(sgw_server);
	uteid = sgwu.generate_uteid(sgwc.ue_num);
	sgwc.create_session_req_to_pgw(uteid);
	sgwc.create_session_res_from_pgw(tun_udata.pgw_uteid);
	sgwc.create_session_res_to_mme(sgw_server);
	sgwc.modify_session_req_from_mme(sgw_server, tun_udata.enodeb_uteid);
	sgwc.modify_session_res_to_mme(sgw_server, uteid);
	sgwc.fill_pgw_addr(tun_udata.pgw_port, tun_udata.pgw_addr);
	sgwc.fill_tun_ctable();
	sgwu.fill_tun_utable(uteid, tun_udata);
	cout<<"Tunnel is formed from eNodeB to PGW via SGW for UE - "<<sgwc.ue_num<<endl;
	sgwc.delete_session_req_from_mme(sgw_server);
	sgwc.delete_session_req_to_pgw();
	sgwc.delete_session_res_from_pgw();
	sgwc.delete_session_res_to_mme(sgw_server);
	sgwu.erase_tun_utable(uteid);
	cout<<"SGW has successfully deallocated resources for UE - "<<sgwc.ue_num<<endl;
}

void handle_udata(Server &sgw_server){
	SGWu sgwu;
	fd_set read_set;
	int max_fd;
	int size;
	int i;
	int status;

	while(1){
		FD_ZERO(&read_set);
		FD_SET(sgw_server.server_socket, &read_set); 
		max_fd = sgw_server.server_socket;
		size = sgwu.pos;
		for(i=0;i<size;i++){
			FD_SET(sgwu.to_pgw[i].client_socket, &read_set); 
			max_fd = max(max_fd, sgwu.to_pgw[i].client_socket);
		}
		status = select(max_fd + 1, &read_set, NULL, NULL, NULL);
		report_error(status, "Select-process failure\tTry again");		
		if(FD_ISSET(sgw_server.server_socket, &read_set)){
			sgwu.recv_enodeb(sgw_server);
			sgwu.set_uteid();
			sgwu.set_tun_udata();
			sgwu.set_pgw_num();
			sgwu.make_data_pgw();
			sgwu.send_pgw();
		}
		for(i=0;i<size;i++){
			if(FD_ISSET(sgwu.to_pgw[i].client_socket, &read_set)){
				sgwu.recv_pgw(i);
				sgwu.set_uteid();
				sgwu.set_tun_udata();
				sgwu.make_data_enodeb();
				sgwu.send_enodeb(sgw_server);
			}
		}
	}
}

int main(int argc, char *argv[]){
	Server sgw_server;
	
	usage(argc, argv);
	sgw_server.begin_thread_pool(2 * atoi(argv[1]), process_traffic);
	sgw_server.fill_server_details(g_sgw1_port, g_sgw1_addr);
	sgw_server.bind_server();
	sgw_server.listen_accept();
	return 0;
}
