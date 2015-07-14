#ifndef PACKET_H
#define PACKET_H

#include "utils.h"

//(C) Operations: Raw socket programming with headers(IP, UDP)
#include <netinet/ip.h>     
#include <netinet/udp.h>    
#include <sys/ioctl.h>      
#include <bits/ioctls.h>    
#include <net/if.h>         

#define DATA_SIZE 100
#define GTPC_LEN 4
#define GTPU_LEN 4
#define IP_LEN 20
#define UDP_LEN 8

struct path{
	int src_port;
	int dst_port;
	const char *src_ip;
	const char *dst_ip;
};

struct gtpc{
	uint16_t user_num;
	uint16_t teid;
};

struct gtpu{
	uint16_t user_num;
	uint16_t teid;
};

struct Packet{
	static const int ip_flags[4];

	char *src_ip;
	char *dst_ip;
	int src_port;
	int dst_port;
	
	struct gtpc gtpc_hdr;
	struct gtpu gtpu_hdr;
	struct ip ip_hdr;
	struct udphdr udp_hdr;

	uint8_t *data;
	uint8_t *packet;
	int data_len;
	int packet_len;
	
	Packet();

	void fill_gtpc_hdr(int);
	void fill_gtpu_hdr(int);
	void fill_ip_hdr(const char*, const char*);
	void fill_udp_hdr(int, int);
	void fill_data(int, int, int);
	void fill_data(int, int, const char*);
	void eval_udp_checksum();
	uint16_t ip_checksum(uint16_t*, int);
	uint16_t udp_checksum();	

	void add_gtpc_hdr();
	void add_gtpu_hdr();
	void rem_gtpc_hdr();
	void rem_gtpu_hdr();
	void encap();
	void decap();
	void clear_data();
	void clear_packet();

	void make_pkt(path);
	~Packet();
};

#endif //PACKET_H