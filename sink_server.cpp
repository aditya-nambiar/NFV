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
	int tnum;
	string command;
	string addr;
	int port;

	tnum = *((int*)arg);
	port = (tnum + 55000);
	addr.assign(g_private_sink_addr);
	command = "iperf3 -s -B " + addr + " -p " + to_string(port);
	system(command.c_str());
}

int main(){
	pthread_t tid[MAX_CONNECTIONS];
	int tnum[MAX_CONNECTIONS];
	pthread_t mon_tid;
	int status;
	int i;

	setup_interface();
	setup_tun();
	status = pthread_create(&mon_tid, NULL, monitor_traffic, NULL);
	report_error(status);	
	for(i=0;i<MAX_CONNECTIONS;i++){
		tnum[i] = i;
		status = pthread_create(&tid[i], NULL, process_traffic, &tnum[i]);
		report_error(status);	
	}
	for(i=0;i<MAX_CONNECTIONS;i++){
		pthread_join(tid[i], NULL);
	}
	return 0;
}