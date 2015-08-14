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
}

void handle_cdata(Server &sgw_server){
	SGWc sgwc;
	SGWu sgwu;
	TunUdata tun_udata;
	uint16_t uteid;

	uteid = sgwu.get_uteid();
	sgwc.create_session_request_from_mme(sgw_server);
	sgwc.create_session_request_to_pgw(uteid);
	sgwc.create_session_response_from_pgw(tun_udata.pgw_uteid);
	sgwc.create_session_response_to_mme(sgw_server);
	sgwc.modify_session_request_from_mme(tun_udata.enodeb_uteid);
	sgwc.modify_session_response_to_mme(sgw_server, uteid);
	tun_data.pgw_port = g_pgw_port;
	strcpy(tun_data.pgw_addr, g_pgw_addr);
	g_tun_table[sgwc.cteid] = sgwc.tun_cdata;
	g_tun_table[uteid] = tun_udata;
	cout<<"Tunnel is formed from eNodeB to PGW via SGW for UE - "<<sgwc.ue_num<<endl;
}

void handle_udata(Server &sgw_server){
	SGWu sgwu;
	
	while(1){
		sgwu.recv_enodeb(sgw_server);
		sgwu.set_uteid();
		sgwu.set_tun_udata();
		sgwu.set_pgw_num();
		sgwu.make_data_pgw();
		sgwu.send_pgw();
		sgwu.recv_pgw();
		sgwu.set_uteid();
		sgwu.set_tun_udata();
		sgwu.make_data_enodeb();
		sgwu.send_enodeb(sgw_server);
	}
}

int main(){
	Server sgw_server;
	
	sgw_server.fill_server_details(g_sgw1_port, g_sgw1_addr);
	sgw_server.bind_server();
	sgw_server.listen_accept(process_traffic);
	return 0;
}