#ifndef SGW_H
#define SGW_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"

void* multithreading_func(void*);
void handle_cdata();
void handle_udata();

#endif //SGW_H