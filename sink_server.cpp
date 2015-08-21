#include "sink_server.h"

void setup_interface(){
	string arg;

	arg = "sudo ifconfig eth0:192.168.100.2/16";
	system(arg.c_str());
	cout<<"Interface successfullly created for Sink"<<endl;
}

void setup_tun(){

	system("sudo openvpn --rmtun --dev tun1");
	system("sudo openvpn --mktun --dev tun1");
	system("sudo ip link set tun1 up");
	system("sudo ip addr add 192.168.100.1/24 dev tun1");
}

void* monitor_traffic(void *arg){
	SinkMonitor sink_monitor;

	SinkMonitor::attach_to_tun();
	SinkMonitor::configure_topgw();
	sink_monitor.listen_accept_pgw();
}

void* process_traffic(void *arg){
	TCPServer sink_server;

	sink_server.server_socket = *(int*)arg;
	sink_server.read_data();
	cout<<"Sent message is "<<sink_server.pkt.data<<endl;
}

int main(){
	TCPServer sink_server;
	pthread_t mon_tid;
	int status;

	setup_interface();
	setup_tun();
	status = pthread_create(&mon_tid, NULL, monitor_traffic, NULL);
	report_error(status);	
	sink_server.fill_server_details(g_private_sink_port, g_private_sink_addr);
	sink_server.bind_server();
	sink_server.listen_accept(process_traffic);
	return 0;
}