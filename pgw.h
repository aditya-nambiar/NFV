#ifndef PGW_H
#define PGW_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "raw_socket.h"

void* multithreading_func(void*);
void handle_cdata();
void handle_udata();

#endif //PGW_H