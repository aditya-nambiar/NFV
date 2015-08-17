#include "pgw_server.h"

void setup_tun(){

	system("sudo openvpn --rmtun --dev tun1");
	system("sudo openvpn --mktun --dev tun1");
	system("sudo ip link set tun1 up");
	system("sudo ip addr add 192.168.100.1/24 dev tun1");
}

void* monitor_traffic(void *arg){
	PWGcMonitor pgwc_monitor;

	pgwc_monitor.attach_to_tun();
	pgwc_monitor.attach_to_sink();
	while(1){
		pgwc_monitor.read_tun();
		pgwc_monitor.write_sink();		
	}
}

void* process_traffic(void *arg){
	int type;
	ClientDetails entity = *(ClientDetails*)arg;
	Server pgw_server;

	pgw_server.fill_server_details(g_freeport, g_pgw_addr);
	pgw_server.bind_server();
	pgw_server.client_sock_addr = entity.client_sock_addr;
	pgw_server.client_num = entity.num;
	pgw_server.connect_with_client();
	pgw_server.read_data();
	memcpy(&type, pgw_server.pkt.data, sizeof(int));
	if(type == 1){
		handle_cdata(pgw_server);
	}
	if(type == 2){
		handle_udata(pgw_server);
	}
}

void handle_cdata(Server &pgw_server){
	PGWc pgwc;
	PGWu pgwu;
	uint16_t uteid;
	TunUdata tun_udata;

	pgwc.create_session_request_from_sgw(pgw_server, tun_udata.sgw_uteid);
	uteid = pgwu.generate(pgwc.ue_num);
	pgwc.create_session_response_to_sgw(pgw_server, uteid);
	pgwc.fill_tun_ctable();
	pgwu.fill_tun_utable(g_ip_table[pgwc.ue_num], tun_udata);
	cout<<"Tunnel created from PGW to SGW for UE - "<<pgwc.ue_num<<endl;
}

void handle_udata(Server &pgw_server){
	PGWu pgwu;

	pgwu.configure_raw_client();
	pgwu.configure_server_for_sink();
	pgwu.recv_sgw(pgw_server);
	pgwu.send_raw_socket();	
	pgwu.recv_sink();
	pgwu.set_ue_ip();
	pgwu.set_tun_udata();
	pgwu.send_sgw(pgw_server);
}

int main(){
	Server pgw_server;
	pthread_t mon_tid;
	int status;

	setup_tun();
	setup_ip_table();
	status = pthread_create(&mon_id, NULL, monitor_traffic, NULL);
	report_error(status);	
	pgw_server.fill_server_details(g_pgw_port, g_pgw_addr);
	pgw_server.bind_server();
	pgw_server.listen_accept(process_traffic);
	free_ip_table();
	return 0;
}