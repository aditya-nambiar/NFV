#include "ran.h"

void* multithreading_func(void *arg){
	int ue_num = *(int*)arg;
	CLIENT user;
	UE ue(ue_num);
	user.input_server_details(mme_port, mme_address);
	user.connect_with_server();
	print_message("UE key is ", ue.key);
	print_message("UE IMSI is ", ue.IMSI);
	print_message("UE MSISDN is ", ue.MSISDN);
}

int main(){
	int status;
	vector<int> ue_num(MAX_UE_COUNT);
	pthread_t tid[MAX_UE_COUNT];
	for(int i=0;i<MAX_UE_COUNT;i++){
		ue_num[i] = i+1;
		status = pthread_create(&tid[i],NULL,multithreading_func,&ue_num[i]);
		report_error(status);
	}
	for(int i=0;i<MAX_UE_COUNT;i++)
		pthread_join(tid[i],NULL);
	return 0;
}
