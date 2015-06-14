#include "server.h"
#include "db.h"

extern const char *query;

void* multithreading_func(void*);
void connect_with_db(DB&);
void fetch_db_data(DB&, const char*);
