#ifndef SINK_SERVER_H
#define SINK_SERVER_H

#include "utils.h"
#include "packet.h"
#include "client.h"
#include "tcp_server.h"
#include "sink_monitor.h"

void setup_interface();
void setup_tun();
void* monitor_traffic(void*);
void* process_traffic(void*);

#endif //SINK_SERVER_H