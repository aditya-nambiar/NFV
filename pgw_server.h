#ifndef PGW_SERVER_H
#define PGW_SERVER_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "raw_socket.h"
#include "pgwc.h"
#include "pgwu.h"
#include "pgwc_monitor.h"

void setup_tun();
void* monitor_traffic(void*);
void* process_traffic(void*);
void handle_cdata(Server&);
void handle_udata(Server&);

#endif //PGW_SERVER_H