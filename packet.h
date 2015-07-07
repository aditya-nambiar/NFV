#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

//(C) Operations: Raw socket programming with headers(IP, UDP)
#include <netinet/ip.h>     
#include <netinet/udp.h>    
#include <sys/ioctl.h>      
#include <bits/ioctls.h>    
#include <net/if.h>         

#define GTPU_LEN 4
#define IP_LEN 20
#define UDP_LEN 8

struct gtpu{
	uint16_t user_num;
	uint16_t teid;
};

struct Packet{
	static int ip_flags[4];

	char *src_ip;
	char *dst_ip;
	int src_port;
	int dst_port;
	
	struct gtpu gtpu_hdr;
	struct ip ip_hdr;
	struct udphdr udp_hdr;

	uint8_t *data, 
	uint8_t *packet;
	int data_len;
	
	Packet();
	void fill_gtpu_hdr(int);
	void fill_ip_hdr(const char*, const char*);
	void fill_udp_hdr(int, int);
	uint16_t ip_checksum(uint16_t, int);
	uint16_t udp_checksum();	

	void add_gtpu_hdr();
	void rem_gtpu_hdr();
	void encap();
	void decap();
	void clear_packet();
	~Packet();
};