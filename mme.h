#ifndef MME_H
#define MME_H

#include "utils.h"
#include "packet.h"
#include "server.h"
#include "client.h"

struct Gateways{
	static const char *sgw1;
	static const char *sgw2;
	static const char *sgw3;
	static const char *pgw;

	static void set_gateways();
};

void* multithreading_func(void*);
void authenticate(Server&, Client&);
void setup_tunnel(Server&, Client&);

#endif //MME_H