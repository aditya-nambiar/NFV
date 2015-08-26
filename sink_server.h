#ifndef SINK_SERVER_H
#define SINK_SERVER_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "client.h"
#include "sink_monitor.h"

#define MAX_THREADS UE_COUNT

void setup_interface();
void setup_tun();
void* monitor_traffic(void*);
void* process_traffic(void*);

#endif //SINK_SERVER_H
