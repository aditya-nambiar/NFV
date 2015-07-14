#ifndef RAN_H
#define RAN_H

#include "utils.h"
#include "packet.h"
#include "client.h"
#include "ue.h"

void* multithreading_func(void*);
void attach_with_mme(UserEquipment&, Client&);
void start_info_flow(UserEquipment&);
void send_ue_data(UserEquipment&);
void receive_reply(UserEquipment&);

#endif //RAN_H