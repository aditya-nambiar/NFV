#ifndef SINK_SERVER_H
#define SINK_SERVER_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "client.h"
#include "sink_monitor.h"

extern int g_total_connections;

void setup_interface();
void setup_tun();
void* monitor_traffic(void*);
void* process_traffic(void*);
void startup_sink(char**, vector<int>&, vector<pthread_t>&);

#endif //SINK_SERVER_H
