#ifndef SGW_SERVER_H
#define SGW_SERVER_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"
#include "sgwc.h"
#include "sgwu.h"

void* process_traffic(void*);
void handle_cdata(Server&, Tunnel&, int&);
void handle_udata(Server&);
void set_bearer_id(int, int);

#endif //SGW_SERVER_H