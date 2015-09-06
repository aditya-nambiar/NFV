#ifndef HSS_H
#define HSS_H

#include "utils.h"
#include "packet.h"
#include "thread_pool.h"
#include "server.h"
#include "db_mysql.h"

extern const char *g_query;

void* multithreading_func(void*);
void authenticate_query(unsigned long long, unsigned long long, Packet&);
void connect_with_db(MySql&);
void fetch_db_data(MySql&, const char*);

#endif //HSS_H