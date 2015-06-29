#include "server.h"
#include "db_mysql.h"

//(C) Operations: Date & Time
#include <ctime>

extern const char *g_query;

void* multithreading_func(void*);
void authenticate_query(unsigned long long, unsigned long long,char*);
void connect_with_db(MySql&);
void fetch_db_data(MySql&, const char*);
void input_db_data();
