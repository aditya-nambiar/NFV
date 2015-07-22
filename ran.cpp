#include "ran.h"

void* multithreading_func(void *arg){
	int ue_num = *(int*)arg;
	Client user;
	user.bind_client();
	user.fill_server_details(g_mme_port, g_mme_addr);
	user.connect_with_server(ue_num);	
	UserEquipment ue(ue_num);
	attach_with_mme(ue, user);
}

void attach_with_mme(UserEquipment &ue, Client &user){
	ue.authenticate(user);
	ue.setup_tunnel(user);
}

void start_info_flow(UserEquipment &ue){

}

void send_ue_data(UserEquipment &ue){

}

void receive_reply(UserEquipment &ue){

}

int main(){
	int status;
	vector<int> ue_num(UE_COUNT);
	pthread_t tid[UE_COUNT];
	for(int i=0;i<UE_COUNT;i++){
		ue_num[i] = i+1;
		status = pthread_create(&tid[i],NULL,multithreading_func,&ue_num[i]);
		report_error(status);
	}
	for(int i=0;i<UE_COUNT;i++)
		pthread_join(tid[i],NULL);
	return 0;
}
