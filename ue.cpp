#include "ue.h"

UserEquipment::UserEquipment(int ue_num){
	key = key_generation(ue_num);
	imsi = key*1000;
	msisdn = 9000000000 + key;
}

void UserEquipment::authenticate(Client &user){
	unsigned long long autn, rand, res;
	char *reply = allocate_str_mem(IP_MAXPACKET);
	type = 1;
	user.pkt.clear_data();
	user.pkt.fill_data(0, sizeof(int), type);
	user.pkt.fill_data(sizeof(int), sizeof(imsi), imsi);
	user.pkt.fill_data(sizeof(int) + sizeof(imsi), sizeof(msisdn), msisdn);
	user.pkt.add_data();
	user.write_data();
	user.read_data();
	memcpy(&autn, user.pkt.data, sizeof(autn));
	memcpy(&rand, user.pkt.data + sizeof(autn), sizeof(rand));
	cout<<autn<<"	"<<rand<<endl;
	res = get_autn_res(autn, rand);
	cout<<"Result is "<<res<<endl;
	user.pkt.clear_data();
	user.pkt.fill_data(0, sizeof(res), res);
	user.pkt.add_data();
	user.write_data();
	user.read_data();
	memcpy(reply, user.pkt.data, user.pkt.data_len);
	cout<<"This is the message -"<<reply<<endl;
	if(strcmp((const char*)reply, "OK") == 0)
		print_message("Authentication Successful for UserEquipment - ", key);
}

unsigned long long UserEquipment::key_generation(int ue_num){
	return ue_num;
}

unsigned long long UserEquipment::get_autn_res(unsigned long long autn, unsigned long long rand){
	unsigned long long res;
	res = autn*key + rand*(key+1);
	return res;
}

UserEquipment::~UserEquipment(){
	// Dummy destructor
}
