#include "sink_monitor.h"

Client SinkMonitor::to_pgw;
int SinkMonitor::tun_fd;
const char* SinkMonitor::tun_name;

SinkMonitor::SinkMonitor(){

	// Dummy
}

void SinkMonitor::attach_to_tun(){	
	struct ifreq ifr;
	const char *dev = "/dev/net/tun";
	int flags;
	int status;

	tun_name = "tun1";
	flags = (IFF_TUN | IFF_NO_PI);
	tun_fd = open(dev , O_RDWR);
	report_error(tun_fd, "Opening /dev/net/tun");
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
	if(*tun_name) {
		strncpy(ifr.ifr_name, tun_name, IFNAMSIZ);
	}
	status = ioctl(tun_fd, TUNSETIFF, (void*)&ifr);
	if(status<0){
		cout<<"ioctl(TUNSETIFF)"<<" "<<errno<<endl;
		close(tun_fd);
		exit(-1);
	}
	// strcpy(tun_name, ifr.ifr_name);
}

void SinkMonitor::read_tun(){

	pkt.clear_data();
	count = read(tun_fd, pkt.data, BUFFER_SIZE);
	report_error(count);
	pkt.data_len = count;
}

void SinkMonitor::write_tun(){

	count = write(tun_fd, pkt.data, pkt.data_len);
	report_error(count);
}

void SinkMonitor::configure_topgw(){

	to_pgw.bind_client();
	to_pgw.fill_server_details(g_pgw_server_for_sink_port, g_pgw_server_for_sink_addr);
}

void SinkMonitor::listen_accept_pgw(){

	for_pgw.fill_server_details(g_public_sink_port, g_public_sink_addr);
	for_pgw.bind_server();
	for_pgw.listen_accept(start_monitor);
	// for_pgw.listen_accept_for_class(boost::bind(&SinkMonitor::start_monitor, this, _1));
}

void SinkMonitor::copy_to_topgwpkt(){

	to_pgw.pkt.clear_data();
	to_pgw.pkt.fill_data(0, pkt.data_len, pkt.data);
}

void SinkMonitor::copy_to_pkt(Packet &arg){

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

SinkMonitor::~SinkMonitor(){

	// Dummy
}

void* start_monitor(void *arg){ 
	SinkMonitor sink_monitor;
	TCPServer monitor;
	Client to_pgw;
	int tun_fd;
	int net_fd;
	int maxfd;
	int status;
	fd_set rd_set;

	tun_fd = sink_monitor.tun_fd;
	to_pgw = sink_monitor.to_pgw;
	monitor.server_socket = *(int*)arg;
	net_fd = monitor.server_socket;
	maxfd = (tun_fd > net_fd)?tun_fd:net_fd;
	while(1){
		FD_ZERO(&rd_set);
		FD_SET(tun_fd, &rd_set); 
		FD_SET(net_fd, &rd_set);
		status = select(maxfd + 1, &rd_set, NULL, NULL, NULL);
		report_error(status, "Select-process failure\tTry again");
		if(FD_ISSET(tun_fd, &rd_set)){
			sink_monitor.read_tun();
			sink_monitor.copy_to_topgwpkt();
			to_pgw.pkt.make_data_packet();
			to_pgw.write_data();
		}
		if(FD_ISSET(net_fd, &rd_set)) {
			monitor.read_data();
			sink_monitor.copy_to_pkt(monitor.pkt);
			sink_monitor.write_tun();
		}
	}
}