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
#define MAX_CONNECTIONS 1000

struct server_details{
	int server_port;
	int server_socket;
	const char *server_address;
};

void report_error(int arg){
	if(arg<0){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}
}

server_details create_server_socket(const char *server_address){
	server_details dts;
	int server_socket, status, opt;
	static int client_num = 0;
	dts.server_port = 5000+client_num;
	dts.server_address = server_address;
	client_num++;
	struct sockaddr_in server_sock_addr;
	dts.server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	report_error(dts.server_socket);
  bzero((char *) &server_sock_addr, sizeof(server_sock_addr));
  server_sock_addr.sin_family = AF_INET;
  server_sock_addr.sin_port = htons(dts.server_port); 
  inet_aton(dts.server_address, &server_sock_addr.sin_addr);
	opt = 1;
	status = setsockopt(dts.server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	report_error(status);
	status = bind(dts.server_socket, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);
	signal(SIGPIPE, SIG_IGN);
	return dts;
}

void* multithreading_func(void *arg){
	sockaddr_in client_sock_addr = *(sockaddr_in*)arg;
	int client_serving_socket, status;
	socklen_t len;
	server_details dts;
	len = sizeof(sockaddr_in);
	cout<<"Coming here2 "<<client_sock_addr.sin_port<<endl;
	dts = create_server_socket("127.0.0.1");
	cout<<"Coming here3 "<<client_sock_addr.sin_port<<endl;
	char server_buffer[BUFFER_SIZE];
	char tem[1024];
	cout<<strlen(dts.server_address)<<endl;
	memcpy(server_buffer, &dts.server_port, sizeof(int));
	memcpy(server_buffer+sizeof(int), dts.server_address, strlen(dts.server_address));
	cout<<"Coming here4 "<<client_sock_addr.sin_port<<endl;
	status = sendto(dts.server_socket, server_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&client_sock_addr, len);
	report_error(status);
	cout<<"Coming here5 "<<client_sock_addr.sin_port<<endl;
	status = recvfrom(dts.server_socket, server_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&client_sock_addr, &len);		
	report_error(status);
	bzero(server_buffer, BUFFER_SIZE);
	strcpy(server_buffer, "OK");
	cout<<"Client processing "<<pthread_self()<<" "<<dts.server_socket<<" "<<client_sock_addr.sin_port<<endl;
	cout<<"Coming here6 "<<client_sock_addr.sin_port<<endl;
	status = sendto(dts.server_socket, server_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&client_sock_addr, len);
	report_error(status);
	cout<<"Coming here7 "<<client_sock_addr.sin_port<<endl;
	close(dts.server_socket);
}

int main(){
	int server_socket, status, i;
	struct sockaddr_in client_sock_addr[MAX_CONNECTIONS];
	char server_buffer[BUFFER_SIZE];
	server_details dts;
	pthread_t tid[MAX_CONNECTIONS];
	socklen_t len;
	len = sizeof(sockaddr_in);
	dts = create_server_socket("127.0.0.01");
	server_socket = dts.server_socket;
	i=0;
	while(1){
		bzero(server_buffer, BUFFER_SIZE);
		cout<<"Waiting here"<<endl;
		status = recvfrom(server_socket, server_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&client_sock_addr[i], &len);		
		report_error(status);
		status = pthread_create(&tid[i],NULL,multithreading_func,&client_sock_addr[i]);
		report_error(status);
		i++;
	}
	close(server_socket);
	return 0;
}
