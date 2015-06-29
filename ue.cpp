#include "ue.h"

UserEquipment::UserEquipment(int ue_num){
	key = key_generation(ue_num);
	imsi = key*1000;
	msisdn = 9000000000 + key;
}

void UserEquipment::authenticate(Client &user){
	unsigned long long autn, rand, res;
	
	type = 1;
	bzero(user.client_buffer, BUFFER_SIZE);
	memcpy(user.client_buffer, &type, sizeof(type));
	memcpy(user.client_buffer+sizeof(type), &imsi, sizeof(imsi));
	memcpy(user.client_buffer+sizeof(type)+sizeof(imsi), &msisdn, sizeof(msisdn));
	user.write_data();

	user.read_data();
	memcpy(&autn, user.client_buffer, sizeof(autn));
	memcpy(&rand, user.client_buffer+sizeof(autn), sizeof(rand));
	cout<<autn<<"	"<<rand<<endl;
	res = get_autn_res(autn, rand);
	cout<<"Result is "<<res<<endl;
	bzero(user.client_buffer, BUFFER_SIZE);
	memcpy(user.client_buffer, &res, sizeof(res));
	user.write_data();
	user.read_data();
	cout<<"This is the message -"<<user.client_buffer<<endl;
	if(strcmp(user.client_buffer, "OK") == 0)
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
