#ifndef PGW_SERVER_H
#define PGW_SERVER_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "raw_socket.h"
#include "pgwc.h"
#include "pgwu.h"

void* process_traffic(void*);
void handle_cdata(Server&);
void handle_udata(Server&);

#endif //PGW_SERVER_H