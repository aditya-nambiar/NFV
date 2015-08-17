#ifndef SINK_MONITOR_H
#define SINK_MONITOR_H

#include "utils.h"
#include "packet.h"
#include "tcp_server.h"
#include "client.h"

struct SinkMonitor{
	TCPServer for_pgw;
	Client to_pgw;
	int tun_fd;
	int count;
	Packet pkt;

	SinkMonitor();
	void attach_to_tun();
	void read_tun();
	void write_tun();
	void begin_topgw();
	void listen_accept_pgw();
	void* start_monitor(void*);
	void copy_to_topgwpkt();
	void copy_to_pkt(Packet&);
	~SinkMonitor();
};

#endif //SINK_MONITOR_H