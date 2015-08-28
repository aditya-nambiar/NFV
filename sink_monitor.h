#ifndef SINK_MONITOR_H
#define SINK_MONITOR_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"

struct SinkMonitor{
	static Client to_pgw;
	static int tun_fd;
	static const char *tun_name;
	Server for_pgw;
	int count;
	Packet pkt;

	SinkMonitor();
	static void attach_to_tun();
	void read_tun();
	void write_tun();
	static void configure_topgw();
	void listen_accept_pgw(int);
	void set_global_variables();
	// void* start_monitor(void*);
	void copy_to_topgwpkt();
	void copy_to_pkt(Packet&);
	~SinkMonitor();
};

extern Client to_pgw;
extern int tun_fd;
extern double g_req_duration;
extern time_t g_start_time;

void* start_monitor(void*);

#endif //SINK_MONITOR_H
