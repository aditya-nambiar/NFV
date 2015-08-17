#include "sink_monitor.h"

SinkMonitor::SinkMonitor(){

	// Dummy
}

void SinkMonitor::attach_to_tun(){	
	struct ifreq ifr;
	const char *dev = "tun1";
	const char *clonedev = "/dev/net/tun";
	int flags;
	int status;

	flags = (IFF_TUN | IFF_NO_PI);
	tun_fd = open(clonedev , O_RDWR);
	report_error(tun_fd, "Opening /dev/net/tun");
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
	if(*dev) {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}
	status = ioctl(tun_fd, TUNSETIFF, (void*)&ifr);
	if(status<0){
		cout<<"ioctl(TUNSETIFF)"<<" "<<errno<<endl;
		close(tun_fd);
		exit(-1);
	}
	strcpy(dev, ifr.ifr_name);
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

void SinkMonitor::begin_topgw(){

	to_pgw.bind_client();
	to_pgw.fill_server_details(g_pgw_server_for_sink_port, g_pgw_server_for_sink_addr);
}

void SinkMonitor::listen_accept_pgw(){

	for_pgw.fill_server_details(g_public_sink_port, g_public_sink_addr);
	for_pgw.bind_server();
	for_pgw.listen_accept(start_monitor);
}

void* SinkMonitor::start_monitor(void *arg){ 
	TCPServer monitor;
	int net_fd;
	int maxfd;
	int status;
	fd_set rd_set;

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
			read_tun();
			copy_to_topgwpkt();
			to_pgw.pkt.make_data_packet();
			to_pgw.write_data();
		}
		if(FD_ISSET(net_fd, &rd_set)) {
			monitor.read_data();
			copy_to_pkt(monitor.pkt);
			write_tun();
		}
	}
}

void copy_to_topgwpkt(){

	to_pgw.pkt.clear_data();
	to_pgw.pkt.fill_data(0, pkt.data_len, pkt.data);
}

void copy_to_pkt(Packet &arg){

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

SinkMonitor::~SinkMonitor(){

	// Dummy
}