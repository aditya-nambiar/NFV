#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "client_ue.h"
using namespace std;

#define BUFFER_SIZE 256
#define UE_COUNT 1

void* multithreading_func(void *arg);

int main(){
	char* server_addr = "localhost";
	int server_port = 1234;
	struct UE_data ue;
	int err;
	pthread_t tid;
	for(int i=1;i<=UE_COUNT;i++){
		cout<<"*********************"<<endl;
		cout<<"Client-"<<i<<" getting connected"<<endl;
		cout<<"*********************"<<endl;
		ue.type = '1';
		ue.IMSI = 1234567812345678;
		ue.MSISDN = 9999999999;
		UE ue1(server_addr, server_port, ue);
		err = pthread_create(&tid,NULL,multithreading_func,&ue1);
		if(err<0){
			cerr<<"Error in thread creation"<<endl;
			exit(-1);
		}
		cout<<"*********************"<<endl<<endl;
	}
	while(1){

	}
	return 0;
}

void* multithreading_func(void *arg){
	UE ue1 = *(UE*)arg;
	char buffer[BUFFER_SIZE];
	char *msg_response;
	if(ue1.write_ue(ue1.packet_attach, sizeof(ue1.packet_attach) + 1) == 0){
		perror("cannot write on socket!");
	}
	msg_response = ue1.read_ue((unsigned char*) buffer, BUFFER_SIZE-1);
	if(strcmp(msg_response,"OK") == 0){
		if(ue1.write_ue(ue1.packet_response, sizeof(ue1.packet_response) + 1) == 0){
			perror("cannot write on socket!");
		}
	}
	msg_response = ue1.read_ue((unsigned char*) buffer, BUFFER_SIZE);
	if(strcmp(msg_response,"OK") == 0){
		cout<<"REGISTRATION SUCCESSFUL!!!"<<endl;
	}
}
