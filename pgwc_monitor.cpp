#include "pgwc_monitor.h"

PGWcMonitor::PGWcMonitor() {

	tun_name = allocate_str_mem(BUFFER_SIZE);
}

PGWcMonitor::PGWcMonitor(const PGWcMonitor &src_obj) {

	tun_name = allocate_str_mem(BUFFER_SIZE);
	to_sink = src_obj.to_sink; 
	tun_fd = src_obj.tun_fd;
	count = src_obj.count;
	strcpy(tun_name, src_obj.tun_name);
	pkt = src_obj.pkt;
}

void swap(PGWcMonitor &src_obj, PGWcMonitor &dst_obj) {
	using std::swap;

	swap(src_obj.to_sink, dst_obj.to_sink); 
	swap(src_obj.tun_fd, dst_obj.tun_fd);
	swap(src_obj.count, dst_obj.count);
	swap(src_obj.tun_name, dst_obj.tun_name);
	swap(src_obj.pkt, dst_obj.pkt);
}

PGWcMonitor& PGWcMonitor::operator=(PGWcMonitor src_obj) {

	swap(*this, src_obj);
	return *this;
}

PGWcMonitor::PGWcMonitor(PGWcMonitor &&src_obj)
	:PGWcMonitor() {

	swap(*this, src_obj);
}

void PGWcMonitor::attach_to_tun() {	
	struct ifreq ifr;
	const char *dev = "/dev/net/tun";
	int flags;
	int status;

	strcpy(tun_name, "tun1");
	flags = (IFF_TUN | IFF_NO_PI);
	tun_fd = open(dev , O_RDWR);
	report_error(tun_fd, "Opening /dev/net/tun");
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
	if (*tun_name) {
		strncpy(ifr.ifr_name, tun_name, IFNAMSIZ);
	}
	status = ioctl(tun_fd, TUNSETIFF, (void*)&ifr);
	if (status < 0) {
		cout << "ioctl(TUNSETIFF)" << " " << errno << endl;
		close(tun_fd);
		exit(-1);
	}
	strcpy(tun_name, ifr.ifr_name);
	cout << "Successfully attached to TUN device" << endl;
}

void PGWcMonitor::read_tun() {

	pkt.clear_data();
	count = read(tun_fd, pkt.data, BUFFER_SIZE);
	report_error(count);
	pkt.data_len = count;
	cout << endl << "Successfully read data from the TUN device" << endl;
}

void PGWcMonitor::write_tun() {

	count = write(tun_fd, pkt.data, pkt.data_len);
	report_error(count);
	cout << "Successfully written data into the TUN device" << endl;
}

void PGWcMonitor::attach_to_sink() {
	int dummy_num = -1;

	to_sink.bind_client();
	to_sink.fill_server_details(g_public_sink_port, g_public_sink_addr);
	to_sink.connect_with_server(dummy_num);
	cout << "Successfully connected with the Sink server" << endl;
}

void PGWcMonitor::write_sink() {
	struct ip *iphdr = allocate_ip_mem(20);
	struct tcphdr *tcp_hdr = allocate_tcp_mem(20);
	char *sink = allocate_str_mem(INET_ADDRSTRLEN);

	copy_to_sinkpkt();

	memcpy(iphdr, to_sink.pkt.data, 20 * sizeof(uint8_t));
	memcpy(tcp_hdr, to_sink.pkt.data + 20 * sizeof(uint8_t), 20 * sizeof(uint8_t));	
	inet_ntop(AF_INET, &(iphdr->ip_dst), sink, INET_ADDRSTRLEN);
	cout << "At PGW: Sink IP is " << sink << endl;
	cout << "TCP destination port is " << ntohs(tcp_hdr->th_dport) << endl;	
	cout << "Size is " << to_sink.pkt.data_len << endl;
	
	to_sink.pkt.make_data_packet();
	to_sink.write_data();		
	cout << "Successfully written data into the Sink" << endl << endl;
	free(iphdr);
	free(tcp_hdr);
	free(sink);
}

void PGWcMonitor::copy_to_sinkpkt() {
	
	to_sink.pkt.clear_data();
	to_sink.pkt.fill_data(0, pkt.data_len, pkt.data);
}

PGWcMonitor::~PGWcMonitor() {

	free(tun_name);
}
