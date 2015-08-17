#ifndef PGWC_MONITOR_H
#define PGWC_MONITOR_H

#include "utils.h"
#include "packet.h"
#include "tcp_client.h"

struct PGWcMonitor{
	TCPClient to_sink; 
	int tun_fd;
	int count;
	Packet pkt;

	PGWcMonitor();
	void attach_to_tun();
	void read_tun();
	void write_tun();
	void attach_to_sink();
	void write_sink();
	void copy_to_sinkpkt();	
	~PGWcMonitor();
};

#endif //PGWC_MONITOR_H