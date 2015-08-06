#ifndef RAN_H
#define RAN_H

#include "utils.h"
#include "packet.h"
#include "client.h"
#include "ue.h"

void setup_tun();
void* multithreading_func(void*);
void attach_with_mme(UserEquipment&, Client&);
void send_traffic(UserEquipment&, int&);

#endif //RAN_H