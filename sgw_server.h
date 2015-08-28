#ifndef SGW_SERVER_H
#define SGW_SERVER_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "client.h"
#include "sgwc.h"
#include "sgwu.h"

extern double g_req_duration;
extern time_t g_start_time;

void* process_traffic(void*);
void handle_cdata(Server&);
void handle_udata(Server&);
void startup_sgw_server(char *argv[]);

#endif //SGW_SERVER_H