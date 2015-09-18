#include "packet.h"

const int Packet::ip_flags[4] = {0, 0, 0, 0};

Packet::Packet() {
	
	src_ip = allocate_str_mem(INET_ADDRSTRLEN);
	dst_ip = allocate_str_mem(INET_ADDRSTRLEN);
	data = allocate_uint8_mem(IP_MAXPACKET);
	packet = allocate_uint8_mem(IP_MAXPACKET);	
	data_len = 0;
	packet_len = 0;
}

Packet::Packet(const Packet &src_obj) {
	
	src_ip = allocate_str_mem(INET_ADDRSTRLEN);
	dst_ip = allocate_str_mem(INET_ADDRSTRLEN);
	data = allocate_uint8_mem(IP_MAXPACKET);
	packet = allocate_uint8_mem(IP_MAXPACKET);	
	strcpy(src_ip, src_obj.src_ip);
	strcpy(dst_ip, src_obj.dst_ip);
	src_port = src_obj.src_port;
	dst_port = src_obj.dst_port;
	gtpc_hdr = src_obj.gtpc_hdr;
	gtpu_hdr = src_obj.gtpu_hdr;
	ip_hdr = src_obj.ip_hdr;
	udp_hdr = src_obj.udp_hdr;
	memcpy(data, src_obj.data, src_obj.data_len);
	memcpy(packet, src_obj.packet, src_obj.packet_len);
	data_len = src_obj.data_len;
	packet_len = src_obj.packet_len;
}

void swap(Packet &src_obj, Packet &dst_obj) {
	using std::swap;

	swap(src_obj.src_ip, dst_obj.src_ip);
	swap(src_obj.dst_ip, dst_obj.dst_ip);
	swap(src_obj.src_port, dst_obj.src_port);
	swap(src_obj.dst_port, dst_obj.dst_port);
	swap(src_obj.gtpc_hdr, dst_obj.gtpc_hdr);
	swap(src_obj.gtpu_hdr, dst_obj.gtpu_hdr);
	swap(src_obj.ip_hdr, dst_obj.ip_hdr);
	swap(src_obj.udp_hdr, dst_obj.udp_hdr);
	swap(src_obj.data, dst_obj.data);
	swap(src_obj.packet, dst_obj.packet);
	swap(src_obj.data_len, dst_obj.data_len);
	swap(src_obj.packet_len, dst_obj.packet_len);
}

Packet& Packet::operator=(Packet src_obj) {
	
	swap(*this, src_obj);
	return *this;	
}

Packet::Packet(Packet &&src_obj)
	:Packet() {

	swap(*this, src_obj);	
}

void Packet::fill_gtpc_hdr(uint16_t teid) {
	
	gtpc_hdr.cteid = teid;
}

void Packet::fill_gtpu_hdr(uint16_t teid) {
	
	gtpu_hdr.uteid = teid;
}

void Packet::fill_ip_hdr(const char *src_ip, const char *dst_ip) {
	
	strcpy(this->src_ip, src_ip);
	strcpy(this->dst_ip, dst_ip);
	ip_hdr.ip_hl = IP_LEN / sizeof(uint32_t);
	ip_hdr.ip_v = 4;
	ip_hdr.ip_tos = 0;
	ip_hdr.ip_len = htons(IP_LEN + UDP_LEN + data_len);
	ip_hdr.ip_id = htons(0);
	ip_hdr.ip_off = htons((ip_flags[0] << 15) + (ip_flags[1] << 14) + (ip_flags[2] << 13) +  ip_flags[3]);
	ip_hdr.ip_ttl = 255;
	ip_hdr.ip_p = IPPROTO_UDP;
	if (inet_pton(AF_INET, this->src_ip, &(ip_hdr.ip_src)) != 1) {
		print("Invaild IP address");
		exit (EXIT_FAILURE);
	}
	if (inet_pton(AF_INET, this->dst_ip, &(ip_hdr.ip_dst)) != 1) {
		print("Invaild IP address");
		exit (EXIT_FAILURE);
	}
	ip_hdr.ip_sum = 0;	
	ip_hdr.ip_sum = ip_checksum((uint16_t*)&ip_hdr, IP_LEN);		
}

void Packet::fill_udp_hdr(int arg_src_port, int arg_dst_port) {
	
	src_port = arg_src_port;
	dst_port = arg_dst_port;
	udp_hdr.source = htons(src_port);
	udp_hdr.dest = htons(dst_port);
	udp_hdr.len = htons(UDP_LEN + data_len);
}

void Packet::fill_data(int pos, int len, int &arg) {
	
	memcpy(data + pos, &arg, len * sizeof(uint8_t));
	data_len += len;
}

void Packet::fill_data(int pos, int len, uint16_t &arg) {
	
	memcpy(data + pos, &arg, len * sizeof(uint8_t));
	data_len += len;
}

void Packet::fill_data(int pos, int len, unsigned long long &arg) {
	
	memcpy(data + pos, &arg, len * sizeof(uint8_t));
	data_len += len;
}

void Packet::fill_data(int pos, int len, uint8_t *message) {
	
	memcpy(data + pos, message, len * sizeof(uint8_t));
	data_len += len;
}

void Packet::fill_data(int pos, int len, const char *message) {
	
	memcpy(data + pos, message, len * sizeof(uint8_t));
	data_len += len;
}

void Packet::fill_data(int pos, int len, string &message) {
	
	memcpy(data + pos, message.c_str(), len * sizeof(uint8_t));
	data_len += len;
}

void Packet::eval_udp_checksum() {
	
	udp_hdr.check = udp_checksum();
}

uint16_t Packet::ip_checksum(uint16_t *addr, int len) {
	int count;
	register uint32_t sum;
	uint16_t answer;
	
	count = len;
	sum = answer = 0;
	while (count > 1) {
		sum += *(addr++);
		count -= 2;
	}
	if (count > 0) {
		sum += *(uint8_t *) addr;
	}
	while (sum >> 16) {
		sum = (sum & 0xffff) + (sum >> 16);
	}
	answer = ~sum;
	return answer;
}

uint16_t Packet::udp_checksum() {
	char buf[IP_MAXPACKET];
	char *ptr;
	int chk_sum_len;
	int i;
  	
  	chk_sum_len = 0;
  	ptr = &buf[0];
  	memcpy(ptr, &ip_hdr.ip_src.s_addr, sizeof (ip_hdr.ip_src.s_addr));
  	ptr += sizeof(ip_hdr.ip_src.s_addr);
  	chk_sum_len += sizeof (ip_hdr.ip_src.s_addr);
  	memcpy(ptr, &ip_hdr.ip_dst.s_addr, sizeof (ip_hdr.ip_dst.s_addr));
  	ptr += sizeof(ip_hdr.ip_dst.s_addr);
  	chk_sum_len += sizeof (ip_hdr.ip_dst.s_addr);
  	*ptr = 0; 
  	ptr++;
  	chk_sum_len += 1;
  	memcpy(ptr, &ip_hdr.ip_p, sizeof (ip_hdr.ip_p));
  	ptr += sizeof (ip_hdr.ip_p);
  	chk_sum_len += sizeof (ip_hdr.ip_p);
  	memcpy(ptr, &udp_hdr.len, sizeof (udp_hdr.len));
  	ptr += sizeof (udp_hdr.len);
  	chk_sum_len += sizeof (udp_hdr.len);
  	memcpy(ptr, &udp_hdr.source, sizeof (udp_hdr.source));
  	ptr += sizeof (udp_hdr.source);
  	chk_sum_len += sizeof (udp_hdr.source);
  	memcpy(ptr, &udp_hdr.dest, sizeof (udp_hdr.dest));
  	ptr += sizeof (udp_hdr.dest);
  	chk_sum_len += sizeof (udp_hdr.dest);
  	memcpy(ptr, &udp_hdr.len, sizeof (udp_hdr.len));
  	ptr += sizeof (udp_hdr.len);
  	chk_sum_len += sizeof (udp_hdr.len);
  	*ptr = 0; 
  	ptr++;
  	*ptr = 0; 
  	ptr++;
  	chk_sum_len += 2;
  	memcpy(ptr, data, data_len);
  	ptr += data_len;
  	chk_sum_len += data_len;
  	for (i = 0; i < data_len%2; i++, ptr++) {
  		*ptr = 0;
  		ptr++;
  		chk_sum_len++;
  	}
  	cout << "In checksum is " << buf << " " << chk_sum_len << endl;
  	return ip_checksum((uint16_t*)buf, chk_sum_len);
}

void Packet::make_data_packet() {
	
	clear_packet();
	memcpy(packet, data, data_len);
	packet_len = data_len;
}

void Packet::add_gtpc_hdr() {
	int len;
	uint8_t *tem;
	
	tem = allocate_uint8_mem(IP_MAXPACKET);
	memcpy(tem, &gtpc_hdr, GTPC_LEN * sizeof(uint8_t));
	memcpy((tem + GTPC_LEN), data, data_len * sizeof(uint8_t));
	len = GTPC_LEN + data_len;
	clear_data();
	fill_data(0, len, tem);
	free(tem);
}

void Packet::add_gtpu_hdr() {
	int len;
	uint8_t *tem;

	tem = allocate_uint8_mem(IP_MAXPACKET);
	memcpy(tem, &gtpu_hdr, GTPU_LEN * sizeof(uint8_t));
	memcpy((tem + GTPU_LEN), data, data_len * sizeof(uint8_t));
	len = GTPU_LEN + data_len;
	clear_data();
	fill_data(0, len, tem);
	free(tem);
}

void Packet::rem_gtpc_hdr() {
	int len;
	uint8_t *tem;	

	tem = allocate_uint8_mem(IP_MAXPACKET);
	memcpy(&gtpc_hdr, data, GTPC_LEN * sizeof(uint8_t));
	memcpy(tem, (data + GTPC_LEN), (data_len - GTPC_LEN) * sizeof(uint8_t));
	len = data_len - GTPC_LEN;
	clear_data();
	fill_data(0, len, tem);
	free(tem);
}

void Packet::rem_gtpu_hdr() {
	int len;
	uint8_t *tem;

	tem = allocate_uint8_mem(IP_MAXPACKET);
	memcpy(&gtpu_hdr, data, GTPU_LEN * sizeof(uint8_t));
	memcpy(tem, (data + GTPU_LEN), (data_len - GTPU_LEN) * sizeof(uint8_t));
	len = data_len - GTPU_LEN;
	clear_data();
	fill_data(0, len, tem);
	free(tem);
}

void Packet::encap() {
	
	clear_packet();
	memcpy(packet, &ip_hdr, IP_LEN * sizeof(uint8_t));
	memcpy((packet + IP_LEN), &udp_hdr, UDP_LEN * sizeof(uint8_t));
	memcpy((packet + IP_LEN + UDP_LEN), data, data_len * sizeof(uint8_t));
	packet_len = IP_LEN + UDP_LEN + data_len;
}

void Packet::decap() {
	
	// Dummy: No decapsulation needed. Raw sockets help in decapsulting outer IP and UDP headers.
	clear_packet();
}

void Packet::clear_data() {
	int len = IP_MAXPACKET;
	
	memset(data, 0, len * sizeof (uint8_t));	
	data_len = 0;
}

void Packet::clear_packet() {
	int len = IP_MAXPACKET;
	
	memset(packet, 0, len * sizeof (uint8_t));	
	packet_len = 0;
}

void Packet::copy_pkts(Packet &dst, Packet &src) {

	dst.clear_data();
	dst.fill_data(0, src.data_len, src.data);
}

void Packet::copy_frompkt(Packet &src) {

	this->clear_data();
	this->fill_data(0, src.data_len, src.data);
}

void Packet::copy_topkt(Packet &dst) {

	dst.clear_data();
	dst.fill_data(0, data_len, data);
}

Packet::~Packet() {
	
	free(src_ip);
	free(dst_ip);
	free(data);
	free(packet);
}