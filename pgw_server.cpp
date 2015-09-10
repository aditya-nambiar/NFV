#include "pgw_server.h"

void setup_tun() {

	system("sudo openvpn --rmtun --dev tun1");
	system("sudo openvpn --mktun --dev tun1");
	system("sudo ip link set tun1 up");
	system("sudo ip addr add 192.168.100.1/24 dev tun1");
}

void* monitor_traffic(void *arg) {
	PGWcMonitor pgwc_monitor;

	pgwc_monitor.attach_to_tun();
	pgwc_monitor.attach_to_sink();
	cout << "PGW Monitoring started " << endl;
	while (1) {
		pgwc_monitor.read_tun();
		pgwc_monitor.write_sink();		
	}
}

void* process_traffic(void *arg) {
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
	if (type == 1) {
		handle_cdata(pgw_server);
	}
	if (type == 2) {
		handle_udata(pgw_server);
	}
}

void handle_cdata(Server &pgw_server) {
	PGWc pgwc;
	PGWu pgwu;
	uint16_t uteid;
	TunUdata tun_udata;

	pgwc.create_session_request_from_sgw(pgw_server, tun_udata.sgw_uteid);
	uteid = pgwu.generate_uteid(pgwc.ue_num);
	pgwc.create_session_response_to_sgw(pgw_server, uteid);
	pgwc.fill_tun_ctable();
	pgwu.fill_tun_utable(g_ip_table[pgwc.ue_num], tun_udata);
	cout << "Tunnel created from PGW to SGW for UE - " << pgwc.ue_num << endl;
	pgwc.delete_session_req_from_sgw(pgw_server);
	pgwc.delete_session_res_to_sgw(pgw_server);
	pgwu.erase_tun_utable(g_ip_table[pgwc.ue_num]);
	cout << "PGW has successfully deallocated resources for UE - " << pgwc.ue_num << endl;
}

void handle_udata(Server &pgw_server) {
	PGWu pgwu;
	fd_set read_set;
	int max_fd;
	int size;
	int i;
	int status;
	
	pgwu.configure_raw_client();
	pgwu.configure_server_for_sink();
	while (1) {
		FD_ZERO(&read_set);
		FD_SET(pgw_server.server_socket, &read_set); 
		FD_SET(pgwu.for_sink.server_socket, &read_set); 
		max_fd = max(pgw_server.server_socket, pgwu.for_sink.server_socket);
		status = select(max_fd + 1, &read_set, NULL, NULL, NULL);
		report_error(status, "Select-process failure\tTry again");		
		if (FD_ISSET(pgw_server.server_socket, &read_set)) {
			pgwu.recv_sgw(pgw_server);
			pgwu.send_raw_socket();		
		}
		if (FD_ISSET(pgwu.for_sink.server_socket, &read_set)) {
			pgwu.recv_sink();
			pgwu.set_ue_ip();
			pgwu.set_tun_udata();
			pgwu.send_sgw(pgw_server);	
		}
	}
}

int main(int argc, char *argv[]) {
	Server pgw_server;
	pthread_t mon_tid;
	int status;

	check_server_usage(argc, argv);
	setup_tun();
	setup_ip_table();
	status = pthread_create(&mon_tid, NULL, monitor_traffic, NULL);
	report_error(status);	
	pgw_server.begin_thread_pool(2 * atoi(argv[1]), process_traffic);
	pgw_server.fill_server_details(g_pgw_port, g_pgw_addr);
	pgw_server.bind_server();
	pgw_server.listen_accept();
	return 0;
}
