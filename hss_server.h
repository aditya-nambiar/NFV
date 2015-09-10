#ifndef HSS_SERVER_H
#define HSS_SERVER_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "db_mysql.h"
#include "hss.h"

void* process_req(void*);

#endif //HSS_SERVER_H