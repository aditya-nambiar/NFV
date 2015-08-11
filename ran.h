#ifndef RAN_H
#define RAN_H

#include "utils.h"
#include "packet.h"
#include "client.h"
#include "ue.h"
#include "enodeb.h"

unordered_map<char*, TunData> g_tun_table;

void setup_tun();
void* monitor_traffic(void*);
void* generate_traffic(void*);
void attach_with_mme(UserEquipment&, Client&);
void send_traffic(UserEquipment&);

#endif //RAN_H