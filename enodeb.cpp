#include "enodeb.h"

unordered_map<string, TunData> g_tun_table;

TunData::TunData() {


}

TunData::TunData(const TunData &src_obj) {

	sgw_uteid = src_obj.sgw_uteid;
	sgw_port = src_obj.sgw_port;
	sgw_addr = src_obj.sgw_addr;
}

void swap(TunData &src_obj, TunData &dst_obj) {
	using std::swap;

	swap(src_obj.sgw_uteid, dst_obj.sgw_uteid);
	swap(src_obj.sgw_port, dst_obj.sgw_port);
	swap(src_obj.sgw_addr, dst_obj.sgw_addr);
}

TunData& TunData::operator=(TunData src_obj) {

	swap(*this, src_obj);
	return *this;
}

TunData::TunData(TunData &&src_obj)
	:TunData() {

	swap(*this, src_obj);
}

TunData::~TunData() {


}

EnodeB::EnodeB() {
	
	socket_table.clear();
	to_sgw.resize(UDP_LINKS);
	pos = 0;
	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	tun_name = allocate_str_mem(BUFFER_SIZE);
}

EnodeB::EnodeB(const EnodeB &src_obj) {

	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	tun_name = allocate_str_mem(BUFFER_SIZE);
	socket_table = src_obj.socket_table;
	to_sgw = src_obj.to_sgw;
	pkt = src_obj.pkt;
	tun_fd = src_obj.tun_fd;
	pos = src_obj.pos;
	num = src_obj.num;
	count = src_obj.count;
	strcpy(ue_ip, src_obj.ue_ip);
	strcpy(tun_name, src_obj.tun_name);
	tun_data = src_obj.tun_data;
}

void swap(EnodeB &src_obj, EnodeB &dst_obj) {
	using std::swap;

	swap(src_obj.socket_table, dst_obj.socket_table);
	swap(src_obj.to_sgw, dst_obj.to_sgw);
	swap(src_obj.pkt, dst_obj.pkt);
	swap(src_obj.tun_fd, dst_obj.tun_fd);
	swap(src_obj.pos, dst_obj.pos);
	swap(src_obj.num, dst_obj.num);
	swap(src_obj.count, dst_obj.count);
	swap(src_obj.ue_ip, dst_obj.ue_ip);
	swap(src_obj.tun_name, dst_obj.tun_name);
	swap(src_obj.tun_data, dst_obj.tun_data);
}

EnodeB& EnodeB::operator=(EnodeB src_obj) {

	swap(*this, src_obj);
	return *this;
}

EnodeB::EnodeB(EnodeB &&src_obj)
	:EnodeB() {

	swap(*this, src_obj);
}

uint16_t EnodeB::generate_uteid(int ue_num) {
	
	return ue_num; //Dummy uteid
}

void EnodeB::attach_to_tun() {	
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
	if (status<0) {
		cout << "ioctl(TUNSETIFF)" << " " << errno << endl;
		close(tun_fd);
		exit(-1);
	}
	strcpy(tun_name, ifr.ifr_name);
	cout << "Enodeb attached to tun device - " << tun_name << endl;
}

void EnodeB::read_tun() {

	pkt.clear_data();
	count = read(tun_fd, pkt.data, BUFFER_SIZE);
	report_error(count);
	pkt.data_len = count;
}

void EnodeB::write_tun() {

	count = write(tun_fd, pkt.data, pkt.data_len);
	report_error(count);
}

void EnodeB::set_ue_ip() {
	struct ip *iphdr = allocate_ip_mem(20);

	memcpy(iphdr, pkt.data, 20 * sizeof(uint8_t));
	inet_ntop(AF_INET, &(iphdr->ip_src), ue_ip, INET_ADDRSTRLEN);
	// cout << "Through tunnel: UE IP is " << ue_ip << endl;
	free(iphdr);
}

void EnodeB::set_tun_data(bool &data_invalid) {
	string ue_ip_str;

	ue_ip_str.assign(ue_ip);
	if (g_tun_table.find(ue_ip_str) != g_tun_table.end()) {
		tun_data = g_tun_table[ue_ip_str];
		data_invalid = false;
	}
	else {
		cout << "Invalid data received!" << endl;
		data_invalid = true;
	}
	// cout << "Details fetched are: " << "UE IP - " << ue_ip_str << " SGW - port " << tun_data.sgw_port << " SGW addr " << tun_data.sgw_addr << endl;
}

void EnodeB::set_sgw_num() {

	if (socket_table.find(tun_data.sgw_addr) != socket_table.end())
		num = socket_table[tun_data.sgw_addr];
	else {
		cout << "At Enodeb: Trying to connect with a new SGW" << endl;
		connect_with_sgw();
		socket_table[tun_data.sgw_addr] = pos;
		pos++;
		num = socket_table[tun_data.sgw_addr];
	}
}

void EnodeB::connect_with_sgw() {

	to_sgw[pos].bind_client();
	to_sgw[pos].fill_server_details(tun_data.sgw_port, tun_data.sgw_addr.c_str());
	to_sgw[pos].connect_with_server(pos);
	handshake_with_sgw();
}

void EnodeB::handshake_with_sgw() {
	int type = 2;

	to_sgw[pos].pkt.clear_data();
	to_sgw[pos].pkt.fill_data(0, sizeof(int), type);
	to_sgw[pos].pkt.make_data_packet();
	to_sgw[pos].write_data();	
}

void EnodeB::make_data() {

	pkt.fill_gtpu_hdr(tun_data.sgw_uteid);
	pkt.add_gtpu_hdr();
}

void EnodeB::send_data() {

	to_sgw[num].pkt.clear_data();
	to_sgw[num].pkt.fill_data(0, pkt.data_len, pkt.data);
	to_sgw[num].pkt.make_data_packet();
	to_sgw[num].write_data();
}

void EnodeB::recv_data(int &sgw_num) {

	to_sgw[sgw_num].read_data();
	pkt.clear_data();	
	pkt.fill_data(0, to_sgw[sgw_num].pkt.data_len, to_sgw[sgw_num].pkt.data);
	pkt.rem_gtpu_hdr();
}

void EnodeB::fill_tun_table(string &ue_ip_arg, TunData &tun_data_arg){

	g_tun_table[ue_ip_arg] = tun_data_arg;
}

void EnodeB::erase_tun_table(string &ue_ip_arg){

	// g_tun_table.erase(ue_ip_arg); /* This is to make the leftover data traffic to reach its intended destination  */
}

EnodeB::~EnodeB() {

	free(ue_ip);
	free(tun_name);
}
