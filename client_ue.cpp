#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include "client_ue.h"
using namespace std;

#define BUFFER_SIZE 256
#define UE_COUNT 12

UE::UE(int key){
	ue.key = key;
	ue.IMSI = 1234567812345678;
	ue.MSISDN = 9999999999;
}

void UE::packetize(){
	memcpy(packet_attach+0, &connect.type, 1);
	memcpy(packet_attach+1, &ue.IMSI, 8);
	memcpy(packet_attach+9, &ue.MSISDN, 8);
	memcpy(packet_response+0, &connect.type, 1);
	memcpy(packet_response+1, &ue.IMSI, 8);
	memcpy(packet_response+9, &ue.MSISDN, 8);
}

void UE::handleErrors(void){
	abort();
}

char* UE::read_ue(unsigned char* buffer, int read_size){
	int res	;
	memset(buffer, 0, sizeof(buffer));
	res = read(connect.sock_fd, buffer, read_size);
	if(res < 0){
		perror("Read error");
		return (char*)buffer;
	}
	cout<<"Received packet: "<<buffer<<endl;
	return (char*)buffer;
}

int UE::write_ue(unsigned char *write_data, int size_data){
	int res;
	res = write(connect.sock_fd, write_data, size_data);
	if(res<0){
		perror("Error writing data");
		return 0;
	}
	return 1;
}

UE::~UE(){
	close(connect.sock_fd);
}

bool connect_with_server(UE &ue){
	int err;
	char* server_addr = "localhost";
	int server_port = 1234;
	struct sockaddr_in serveraddr;
	int serveraddr_len, res;
	struct hostent *host;
	ue.connect.type = 1;
	ue.connect.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	ue.connect.tid = pthread_self();
	if(ue.connect.sock_fd<0){
		perror("Cannot create a socket");
		return false;
	}
	memset(&serveraddr, 0, sizeof(serveraddr));
	host = gethostbyname(server_addr);
	if(host == NULL){
		perror("Cannot define host address");
	}
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(server_port);
	printf(
	"Server addr = %d.%d.%d.%d, Port = %d\n",
	host->h_addr_list[0][0] & 0xff,
	host->h_addr_list[0][1] & 0xff,
	host->h_addr_list[0][2] & 0xff,
	host->h_addr_list[0][3] & 0xff,
	(int)server_port
	);
	memmove(&(serveraddr.sin_addr.s_addr), host->h_addr_list[0], 4);
	signal(SIGPIPE, SIG_IGN);
	res = connect(ue.connect.sock_fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
	if (res < 0) {
		perror("Cannot connect");
		return false;
	}
	cout<<"Client-"<<ue.ue.key<<" Connected with server\n";
	return true;
}

void* multithreading_func(void *arg){
	int num = *((int*)(arg));
	cout<<num<<endl;
	UE ue(num);
	if(connect_with_server(ue)){
		char buffer[BUFFER_SIZE];
		char *msg_response;
		cout<<"Client-"<<num<<" Sockfd is "<<ue.connect.sock_fd<<endl;
		cout<<"Client-"<<num<<" processing step-1"<<endl;
		ue.packetize();
		if(ue.write_ue(ue.packet_attach, sizeof(ue.packet_attach) + 1) == 0){
			perror("cannot write on socket!");
			return 0;
		}
		cout<<"Client-"<<num<<" processing step-2"<<endl;
		msg_response = ue.read_ue((unsigned char*) buffer, BUFFER_SIZE-1);
		if(strcmp(msg_response,"OK") == 0){
			if(ue.write_ue(ue.packet_response, sizeof(ue.packet_response) + 1) == 0){
				perror("cannot write on socket!");
				return 0;
			}
		}
		cout<<"Client-"<<num<<" processing step-3"<<endl;
		msg_response = ue.read_ue((unsigned char*) buffer, BUFFER_SIZE);
		if(strcmp(msg_response,"OK") == 0){
			cout<<"REGISTRATION SUCCESSFUL for client-"<<ue.ue.key<<endl;
		}else{
			cout<<"No OK reply"<<endl;
		}
	}
}

int main(){
	int err;
	pthread_t tid[UE_COUNT];
	for(int i=0;i<UE_COUNT;i++){
		cout<<"*********************"<<endl;
		cout<<"Client-"<<i+1<<" getting connected"<<endl;
		cout<<"*********************"<<endl;
		err = pthread_create(&tid[i],NULL,multithreading_func,&i);
		if(err<0){
			cerr<<"Error in thread creation"<<endl;
			exit(-1);
		}	
	}
	for(int i=0;i<UE_COUNT;i++)
		pthread_join(tid[i],NULL);
	return 0;
}
