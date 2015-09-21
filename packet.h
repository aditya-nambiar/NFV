#ifndef PACKET_H
#define PACKET_H

#include "utils.h"

#define GTPC_LEN 4
#define GTPU_LEN 4
#define IP_LEN 20
#define UDP_LEN 8

struct GTPc {
	uint16_t cteid;
};

struct GTPu { 
	uint16_t uteid;
};

struct Packet {
	static const int ip_flags[4];

	char *src_ip;
	char *dst_ip;
	int src_port;
	int dst_port;
	
	struct GTPc gtpc_hdr;
	struct GTPu gtpu_hdr;
	struct ip ip_hdr;
	struct udphdr udp_hdr;

	uint8_t *data;
	uint8_t *packet;
	int data_len;
	int packet_len;
	
	Packet();
	Packet(const Packet&);
	friend void swap(Packet&, Packet&);
	Packet& operator=(Packet);
	Packet(Packet&&);

	void fill_gtpc_hdr(uint16_t);
	void fill_gtpu_hdr(uint16_t);
	void fill_ip_hdr(const char*, const char*);
	void fill_udp_hdr(int, int);
	void fill_data(int, int, int);
	void fill_data(int, int, uint16_t);
	void fill_data(int, int, unsigned long long);
	void fill_data(int, int, uint8_t*);	
	void fill_data(int, int, const char*);	
	void fill_data(int, int, string);
	void eval_udp_checksum();
	uint16_t ip_checksum(uint16_t*, int);
	uint16_t udp_checksum();	

	void make_data_packet();
	void add_gtpc_hdr();
	void add_gtpu_hdr();
	void rem_gtpc_hdr();
	void rem_gtpu_hdr();
	void encap();
	void decap();
	void clear_data();
	void clear_packet();
	void copy_pkts(Packet&, Packet&);
	void copy_frompkt(Packet&);
	void copy_topkt(Packet&);
	
	~Packet();
};

#endif //PACKET_H
