#ifndef RAN_H
#define RAN_H

#include "utils.h"
#include "packet.h"
#include "client.h"
#include "ue.h"
#include "enodeb.h"

extern int g_total_connections;

void setup_tun();
void* monitor_traffic(void*);
void* generate_traffic(void*);
void attach_with_mme(UserEquipment&, Client&);
void send_traffic(UserEquipment&);
void startup_ran(char**, vector<int>&, vector<pthread_t>&);

#endif //RAN_H