#ifndef MME_SERVER_H
#define MME_SERVER_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "client.h"
#include "mme.h"

void* process_req(void*);
void attach_ue(MME&);
void recv_req_from_ue(MME&);
void authenticate_ue(MME&);
void setup_tunnel(MME&);
void detach_ue(MME&);

#endif //MME_SERVER_H