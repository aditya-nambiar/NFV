#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

class RawSocket{
public:
	static const int on = 1;
	static const char *interface;
	static struct ifreq ifr;
	static void set_interface(const char*);

	int status;
	int raw_socket;
	int src_port;
	char *src_addr;

	int dst_port;
	char *dst_addr;
	struct sockaddr_in dst_sock_addr;

	RawSocket();
	void src_details(int, const char*);
	void dst_details(int, const char*);
	void bind_client();
	void write_data(Packet&);	
	~RawSocket();		
};