//(C++) Operations: Input/Output
#include <iostream>

//(C++) STL Operations: String, Vector, String stream
#include <string>
#include <vector>
#include <sstream>

//(C) Operations: Input/Output, String, Standard libraries(like atoi, malloc)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//(C) Operations: Socket programming
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>

//(C) Operations: Multithreading
#include <pthread.h>

using namespace std;

#define BUFFER_SIZE 10240
#define UE_COUNT 300

void report_error(int arg){
	if(arg<0){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}
}

const char* to_char_array(unsigned long long arg){
	string tem;
	stringstream out;
	out<<arg;
	tem = out.str();
	const char *ans = tem.c_str();
	return ans;
}

sockaddr_in fill_server_details(int server_port, const char *server_address){
	int status;
	struct sockaddr_in server_sock_addr;	
	bzero((char*)&server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;
	server_sock_addr.sin_port = htons(server_port);
	status = inet_aton(server_address, &server_sock_addr.sin_addr);
	if(status == 0){
		cout<<"ERROR: Invalid IP address"<<endl;
		exit(EXIT_FAILURE);
	}
	return server_sock_addr;
}

void* multithreading_func(void *arg){
	static int i=1;
	int ue_num = *(int*)arg;
	int client_socket;
	struct sockaddr_in server_sock_addr;	
	char client_buffer[BUFFER_SIZE];
	int server_port, status;
	char server_address[BUFFER_SIZE];
	socklen_t len;
	len = sizeof(sockaddr_in);
	
	client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	report_error(client_socket);
	server_port = 5000;
	strcpy(server_address, "127.0.0.1");
	server_sock_addr = fill_server_details(server_port, server_address);
	//cout<<"Coming here1 "<<ue_num<<" "<<server_sock_addr.sin_port<<endl;
	signal(SIGPIPE, SIG_IGN);
	strcpy(client_buffer, "Hi");
	//cout<<"Coming here2 "<<ue_num<<" "<<server_sock_addr.sin_port<<endl;
	status = sendto(client_socket, client_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&server_sock_addr, len);
	report_error(status);
	
	bzero(client_buffer, BUFFER_SIZE);
	//cout<<"Coming here3 "<<ue_num<<" "<<server_sock_addr.sin_port<<endl;
	status = recvfrom(client_socket, client_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&server_sock_addr, &len);
	report_error(status);
	//cout<<ntohs(server_sock_addr.sin_port)<<endl;
	memcpy(&server_port, client_buffer, sizeof(int));
	memcpy(server_address, client_buffer+sizeof(int), sizeof(client_buffer)-sizeof(int));
	server_sock_addr = fill_server_details(server_port, server_address);
	//cout<<"Coming here4 "<<ue_num<<" "<<server_sock_addr.sin_port<<endl;
	const char *tem = to_char_array(ue_num);
	bzero(client_buffer, BUFFER_SIZE);
	strcpy(client_buffer, tem);
	//cout<<"Coming here5 "<<ue_num<<" "<<server_sock_addr.sin_port<<endl;
	status = sendto(client_socket, client_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&server_sock_addr, len);
	report_error(status);
	//cout<<"Coming here6 "<<ue_num<<" "<<server_sock_addr.sin_port<<endl;
	bzero(client_buffer, BUFFER_SIZE);
	status = recvfrom(client_socket, client_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&server_sock_addr, &len);
	//cout<<"Coming here7 "<<ue_num<<" "<<server_sock_addr.sin_port<<endl;
	report_error(status);
	//cout<<"Client-"<<ue_num<<"with port"<<server_port<<" got the reply - "<<client_buffer<<endl;
	cout<<"Closing client "<<i<<endl;
	i++;
	close(client_socket);
}

int main(){
	int status;
	vector<int> ue_num(UE_COUNT);
	pthread_t tid[UE_COUNT];
	for(int i=0;i<UE_COUNT;i++){
		ue_num[i] = i+1;
		status = pthread_create(&tid[i],NULL,multithreading_func,&ue_num[i]);
	}
	for(int i=0;i<UE_COUNT;i++)
		pthread_join(tid[i],NULL);
	return 0;
}
