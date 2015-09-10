#ifndef RAW_SOCKET_H
#define RAW_SOCKET_H

#include "utils.h"
#include "packet.h"

class RawSocket {
public:
	static const int on = 1;
	static const char *interface;
	static struct ifreq ifr;
	static void set_interface(const char*);

	int status;
	int raw_socket;
	// int src_port;
	// char *src_addr;
	Packet pkt;

	// int dst_port;
	// char *dst_addr;
	struct sockaddr_in dst_sock_addr;

	RawSocket();
	RawSocket(const RawSocket&);
	friend void swap(RawSocket&, RawSocket&);
	RawSocket& operator=(RawSocket);
	RawSocket(RawSocket&&);
	void bind_client();	
	// void fill_traffic_details(Packet&);
	// void fill_src_details();
	void fill_dst_details();
	void write_data();	
	~RawSocket();		
};

#endif //RAW_SOCKET_H