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
	sgwc.copy_data(sgw_server.pkt);
	

	
	sgwc.set_ue_num();
	sgwc.set_bearer_id();
	sgwc.set_cteid();
	sgwc.set_mme_cteid();
	sgwc.connect_with_sgw();


	// to_pgw.bind_client();
	// to_pgw.fill_server_details(g_pgw_port, g_pgw_addr);
	// to_pgw.connect_with_server(ue_num);
	// memcpy(&ue_num, sgw_server.pkt.data + sizeof(int), sizeof(int));
	// memcpy(&bearer_id, sgw_server.pkt.data + 2*sizeof(int), sizeof(int));
	memcpy(&tun_data.mme_cteid, sgw_server.pkt.data + 3*sizeof(int), sizeof(uint16_t));
	// set_bearer_id(ue_num, bearer_id);
	// to_pgw.pkt.clear_data();
	// to_pgw.pkt.fill_data(0, sizeof(int), type);



	// to_pgw.pkt.fill_data(sizeof(int), sizeof(int), ue_num);
	// to_pgw.pkt.fill_data(2*sizeof(int), sizeof(int), bearer_id);
	// to_pgw.pkt.fill_data(3*sizeof(int), sizeof(uint16_t), sgw_cteid);
	// to_pgw.pkt.fill_data(3*sizeof(int) + sizeof(uint16_t), sizeof(uint16_t), sgw_uteid);
	// to_pgw.pkt.make_data_packet();
	// to_pgw.write_data();
	// to_pgw.read_data();
	// to_pgw.pkt.rem_gtpc_hdr();
	// memcpy(&tun_data.pgw_cteid, to_pgw.pkt.data, sizeof(uint16_t));
	// memcpy(&tun_data.pgw_uteid, to_pgw.pkt.data + sizeof(uint16_t), sizeof(uint16_t));
	// memcpy(ue_ip_addr, to_pgw.pkt.data + 2*sizeof(uint16_t), INET_ADDRSTRLEN);
	strcpy(reply, "OK");
	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, sizeof(uint16_t), sgw_cteid);
	sgw_server.pkt.fill_data(sizeof(uint16_t), strlen(reply), reply);
	sgw_server.pkt.fill_gtpc_hdr(tun_data.mme_cteid);
	sgw_server.pkt.add_gtpc_hdr();
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
	sgw_server.read_data();
	sgw_server.pkt.rem_gtpc_hdr();
	memcpy(&tun_data.enodeb_uteid, sgw_server.pkt.data, sizeof(uint16_t));
	sgw_server.pkt.clear_data();
	sgw_server.pkt.fill_data(0, sizeof(uint16_t), tun_data.sgw_uteid);
	sgw_server.pkt.fill_data(sizeof(uint16_t), INET_ADDRSTRLEN, ue_ip_addr);
	sgw_server.pkt.fill_data(sizeof(uint16_t) + INET_ADDRSTRLEN, strlen(reply), reply);
	sgw_server.pkt.add_gtpc_hdr();
	sgw_server.pkt.make_data_packet();
	sgw_server.write_data();
	cout<<"Tunnel is formed from eNodeB to PGW via SGW for UE - "<<ue_num<<endl;

	tun_data.pgw_port = g_pgw_port;
	strcpy(tun_data.pgw_addr, g_pgw_addr);

	g_tun_table[sgw_uteid] = tun_data;

	free(ue_ip_addr);
	free(reply);
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