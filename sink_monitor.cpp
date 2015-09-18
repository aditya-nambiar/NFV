#include "sink_monitor.h"

Client SinkMonitor::to_pgw;
int SinkMonitor::tun_fd;
const char* SinkMonitor::tun_name;

SinkMonitor::SinkMonitor() {

	// Dummy
}

SinkMonitor::SinkMonitor(const SinkMonitor &src_obj) {

	for_pgw = src_obj.for_pgw;
	count = src_obj.count;
	pkt = src_obj.pkt;
}

void swap(SinkMonitor &src_obj, SinkMonitor &dst_obj) {
	using std::swap;

	swap(src_obj.for_pgw, dst_obj.for_pgw);
	swap(src_obj.count, dst_obj.count);
	swap(src_obj.pkt, dst_obj.pkt);
}

SinkMonitor& SinkMonitor::operator=(SinkMonitor src_obj) {

	swap(*this, src_obj);
	return *this;
}

SinkMonitor::SinkMonitor(SinkMonitor &&src_obj)
	:SinkMonitor() {

	swap(*this, src_obj);
}

void SinkMonitor::attach_to_tun() {	
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
	if (*tun_name) {
		strncpy(ifr.ifr_name, tun_name, IFNAMSIZ);
	}
	status = ioctl(tun_fd, TUNSETIFF, (void*)&ifr);
	if (status<0) {
		cout << "ioctl(TUNSETIFF)" << " " << errno << endl;
		close(tun_fd);
		exit(-1);
	}
	// strcpy(tun_name, ifr.ifr_name);
	cout << "Sink Monitor attached to TUN device" << endl;
}

void SinkMonitor::read_tun() {

	pkt.clear_data();
	count = read(tun_fd, pkt.data, BUFFER_SIZE);
	report_error(count);
	pkt.data_len = count;
	// cout << "Successfully read data from the TUN device" << endl;
}

void SinkMonitor::write_tun() {

	count = write(tun_fd, pkt.data, pkt.data_len);
	report_error(count);
	// cout << "Successfully written data into the TUN device" << endl;
}

void SinkMonitor::configure_topgw() {

	to_pgw.bind_client();
	to_pgw.fill_server_details(g_pgw_server_for_sink_port, g_pgw_server_for_sink_addr);
	cout << "Configured the server to_pgw successfully " << endl;
}

void SinkMonitor::listen_accept_pgw(int total_connections) {

	for_pgw.set_total_connections(total_connections);
	for_pgw.fill_server_details(g_public_sink_port, g_public_sink_addr);
	for_pgw.bind_server();
	for_pgw.listen_accept(start_monitor);
	// for_pgw.listen_accept_for_class(boost::bind(&SinkMonitor::start_monitor, this, _1));
}

void SinkMonitor::copy_to_topgwpkt() {

	to_pgw.pkt.clear_data();
	to_pgw.pkt.fill_data(0, pkt.data_len, pkt.data);	
}

void SinkMonitor::copy_to_pkt(Packet &arg) {

	pkt.clear_data();
	pkt.fill_data(0, arg.data_len, arg.data);
}

SinkMonitor::~SinkMonitor() {

	// Dummy
}

void* start_monitor(void *arg) { 
	SinkMonitor sink_monitor;
	Server monitor;
	Client to_pgw;
	ClientDetails entity;
	int tun_fd;
	int net_fd;
	int maxfd;
	int status;
	fd_set rd_set;

	struct ip *iphdr = allocate_ip_mem(20);
	struct tcphdr *tcp_hdr = allocate_tcp_mem(20);
	char *sink = allocate_str_mem(INET_ADDRSTRLEN);

	cout << "Successfully started the Sink Monitor" << endl;
	tun_fd = sink_monitor.tun_fd;
	to_pgw = sink_monitor.to_pgw;
	entity = *((ClientDetails*)arg);
	monitor.fill_server_details(g_freeport, g_public_sink_addr);
	monitor.bind_server();
	monitor.client_sock_addr = entity.client_sock_addr;
	monitor.connect_with_client();
	net_fd = monitor.server_socket;
	maxfd = (tun_fd > net_fd)?tun_fd:net_fd;
	while (1) {
		FD_ZERO(&rd_set);
		FD_SET(tun_fd, &rd_set); 
		FD_SET(net_fd, &rd_set);
		status = select(maxfd + 1, &rd_set, NULL, NULL, NULL);
		report_error(status, "Select-process failure\tTry again");
		if (FD_ISSET(tun_fd, &rd_set)) {
			//cout << endl << "Detected data from private sink" << endl;
			sink_monitor.read_tun();
			to_pgw.pkt.clear_data();
			to_pgw.pkt.fill_data(0, sink_monitor.pkt.data_len, sink_monitor.pkt.data);			
			
			memcpy(iphdr, to_pgw.pkt.data, 20 * sizeof(uint8_t));
			memcpy(tcp_hdr, to_pgw.pkt.data + 20 * sizeof(uint8_t), 20 * sizeof(uint8_t));	
			inet_ntop(AF_INET, &(iphdr->ip_dst), sink, INET_ADDRSTRLEN);
			//cout << "At Sink Monitor(From Sink To PGW): UE IP is " << sink << endl;
			//cout << "TCP destination port is " << ntohs(tcp_hdr->th_dport) << endl;
			//cout << "Size is " << to_pgw.pkt.data_len << endl;
			//cout << "Address of to_pgw server is " << to_pgw.server_addr << endl;
			//cout << "Port of to_pgw server is " << to_pgw.server_port << endl;
						
			to_pgw.pkt.make_data_packet();
			to_pgw.write_data();
			//cout << "Successfully written to PGW" << endl << endl;
		}
		if (FD_ISSET(net_fd, &rd_set)) {
			//cout << endl << "Detected data from the PGW" << endl;
			monitor.read_data();
			sink_monitor.copy_to_pkt(monitor.pkt);
			
			memcpy(iphdr, sink_monitor.pkt.data, 20 * sizeof(uint8_t));
			memcpy(tcp_hdr, sink_monitor.pkt.data + 20 * sizeof(uint8_t), 20 * sizeof(uint8_t));	
			inet_ntop(AF_INET, &(iphdr->ip_dst), sink, INET_ADDRSTRLEN);
			//cout << "At Sink Monitor: Sink IP is " << sink << endl;
			//cout << "TCP destination port is " << ntohs(tcp_hdr->th_dport) << endl;	
			//cout << "Size is " << sink_monitor.pkt.data_len << endl;
			
			sink_monitor.write_tun();
			//cout << "Successfully written to private sink" << endl << endl;
		}
	}
	free(iphdr);
	free(tcp_hdr);
	free(sink);
}
