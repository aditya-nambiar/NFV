#include "packet.h"

int Packet::ip_flags = {0, 0, 0, 0};

Packet::Packet(){
	src_ip = allocate_str_mem(INET_ADDRSTRLEN);
	dst_ip = allocate_str_mem(INET_ADDRSTRLEN);
	data = allocate_uint8_mem(IP_MAXPACKET);
	packet = allocate_uint8_rmem(IP_MAXPACKET);	
}

void Packet::fill_gtpu_hdr(int teid){
	gtpu_hdr.user_num = teid;
	gtpu_hdr.teid = teid;
}

void Packet::fill_ip_hdr(const char *src_ip, const char *dst_ip){
	strcpy (this->src_ip, src_ip);
	strcpy (this->dst_ip, dst_ip);
	ip_hdr.ip_hl = IP_LEN / sizeof(uint32_t);
	ip_hdr.ip_v = 4;
	ip_hdr.ip_tos = 0;
	ip_hdr.ip_len = htons(IP_LEN + UDP_LEN + data_len);
	ip_hdr.ip_id = htons(0);
	ip_hdr.ip_off = htons((ip_flags[0] << 15) + (ip_flags[1] << 14) + (ip_flags[2] << 13) +  ip_flags[3]);
	ip_hdr.ip_ttl = 255;
	ip_hdr.ip_p = IPPROTO_UDP;
	if(inet_pton(AF_INET, this->src_ip, &(ip_hdr.ip_src)) != 1){
		print("Invaild IP address");
		exit (EXIT_FAILURE);
	}
	if(inet_pton(AF_INET, this->dst_ip, &(ip_hdr.ip_dst)) != 1){
		print("Invaild IP address");
		exit (EXIT_FAILURE);
	}
	ip_hdr.ip_sum = 0;	
	ip_hdr.ip_sum = checksum ((uint16_t *) &ip_hdr, IP4_HDRLEN);
}

void Packet::fill_udp_hdr(int src_port, int dst_port){
	this->src_port = src_port;
	this->dst_port = dst_port;
	udp_hdr.source = htons(this->src_port);
	udp_hdr.dest = htons(this->dst_port);
	udp_hdr.len = htons(UDP_LEN + data_len);
	udp_hdr.check = udp4_checksum(ip_hdr, udp_hdr, data, data_len);
}

uint16_t Packet::ip_checksum(uint16_t *addr, int len){
	int count;
	register uint32_t sum;
	uint16_t answer;
	len = IP4_HDRLEN;
	count = len;
	sum = answer = 0;
	while(count > 1){
		sum += *(addr++);
		count -= 2;
	}
	if (count > 0) {
		sum += *(uint8_t *) addr;
	}
	while(sum >> 16){
		sum = (sum & 0xffff) + (sum >> 16);
	}
	answer = ~sum;
	return answer;
}

uint16_t Packet::udp_checksum(){
	char buf[IP_MAXPACKET];
	char *ptr;
	int chk_sum_len = 0;
	int i;
  	ptr = &buf[0];
  	memcpy (ptr, &ip_hdr.ip_src.s_addr, sizeof (ip_hdr.ip_src.s_addr));
  	ptr += sizeof (ip_hdr.ip_src.s_addr);
  	chk_sum_len += sizeof (ip_hdr.ip_src.s_addr);
  	memcpy (ptr, &ip_hdr.ip_dst.s_addr, sizeof (ip_hdr.ip_dst.s_addr));
  	ptr += sizeof (ip_hdr.ip_dst.s_addr);
  	chk_sum_len += sizeof (ip_hdr.ip_dst.s_addr);
  	*ptr = 0; 
  	ptr++;
  	chk_sum_len += 1;
  	memcpy (ptr, &ip_hdr.ip_p, sizeof (ip_hdr.ip_p));
  	ptr += sizeof (ip_hdr.ip_p);
  	chk_sum_len += sizeof (ip_hdr.ip_p);
  	memcpy (ptr, &udp_hdr.len, sizeof (udp_hdr.len));
  	ptr += sizeof (udp_hdr.len);
  	chk_sum_len += sizeof (udp_hdr.len);
  	memcpy (ptr, &udp_hdr.source, sizeof (udp_hdr.source));
  	ptr += sizeof (udp_hdr.source);
  	chk_sum_len += sizeof (udp_hdr.source);
  	memcpy (ptr, &udp_hdr.dest, sizeof (udp_hdr.dest));
  	ptr += sizeof (udp_hdr.dest);
  	chk_sum_len += sizeof (udp_hdr.dest);
  	memcpy (ptr, &udp_hdr.len, sizeof (udp_hdr.len));
  	ptr += sizeof (udp_hdr.len);
  	chk_sum_len += sizeof (udp_hdr.len);
  	*ptr = 0; 
  	ptr++;
  	chk_sum_len += 2;
  	memcpy (ptr, data, data_len);
  	ptr += data_len;
  	chk_sum_len += data_len;
  	for (i=0; i<data_len%2; i++, ptr++) {
  		*ptr = 0;
  		ptr++;
  		chk_sum_len++;
  	}
  	return checksum((uint16_t*)buf, chk_sum_len);
}

void Packet::add_gtpu_hdr(){
	uint8_t *tem = allocate_uint8_mem(IP_MAXPACKET);
	memcpy(tem, &gtpu_hdr, GTPU_LEN * sizeof(uint8_t));
	memcpy((tem + GTPU_LEN), data, data_len * sizeof(uint8_t));
	data = tem;
	data_len+= GTPU_LEN;
	free(tem);
}

void Packet::rem_gtpu_hdr(){
	uint8_t *tem = allocate_uint8_mem(IP_MAXPACKET);
	memcpy(&gtpu_hdr, data, GTPU_LEN * sizeof(uint8_t));
	memcpy(tem, (data + GTPU_LEN), (data_len - GTPU_LEN) * sizeof(uint8_t));
	data = tem;
	data_len-= GTPU_LEN;
	free(tem);
}

void Packet::encap(){
	memcpy(packet, &ip_hdr, IP_LEN * sizeof(uint8_t));
	memcpy((packet + IP_LEN), &udp_hdr, UDP_LEN * sizeof(uint8_t));
	memcpy((packet + IP_LEN + UDP_LEN), data, data_len * sizeof(uint8_t));
}

void Packet::decap(){
	memcpy(&ip_hdr, packet, IP_LEN * sizeof(uint8_t));
	memcpy(&udp_hdr, (packet + IP_LEN), UDP_LEN * sizeof(uint8_t));
	memcpy(data, (packet + IP_LEN + UDP_LEN), data_len * sizeof(uint8_t));
}

void Packet::clear_packet(){
	int len = IP_MAXPACKET;
	memset(packet, 0, len * sizeof (uint8_t));	
}

Packet::~Packet(){
	free(src_ip);
	free(dst_ip);
	free(data);
	free(packet);
}