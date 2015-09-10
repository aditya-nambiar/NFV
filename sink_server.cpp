#include "sink_server.h"

int g_total_connections;

void setup_interface(){
	string arg;

	arg = "sudo ifconfig eth0:0 192.168.100.2/16";
	cout<<arg<<endl;
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
	sink_monitor.listen_accept_pgw(g_total_connections);
}

void* process_traffic(void *arg){
	int tnum;
	string command;
	string addr;
	string mtu;
	int port;

	tnum = *((int*)arg);
	port = (tnum + 55000);
	addr.assign(g_private_sink_addr);
	mtu = " -M 1000";
	command = "iperf3 -s -B " + addr + " -p " + to_string(port);
	cout<<command<<endl;
	system(command.c_str());
}

void startup_sink(char *argv[], vector<int> &tnum, vector<pthread_t> &tid){

	g_total_connections = atoi(argv[1]);
	tnum.resize(g_total_connections);
	tid.resize(g_total_connections);
}

int main(int argc, char *argv[]){
	pthread_t mon_tid;
	vector<int> tnum;
	vector<pthread_t> tid;
	int status;
	int i;

	check_server_usage(argc, argv);
	startup_sink(argv, tnum, tid);
	setup_interface();
	setup_tun();
	status = pthread_create(&mon_tid, NULL, monitor_traffic, NULL);
	report_error(status);	
	for(i=0;i<g_total_connections;i++){
		tnum[i] = i;
		status = pthread_create(&tid[i], NULL, process_traffic, &tnum[i]);
		report_error(status);	
	}
	for(i=0;i<g_total_connections;i++){
		pthread_join(tid[i], NULL);
	}
	return 0;
}
