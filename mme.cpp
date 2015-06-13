#include "server.h"

void report_error(int arg){
	if(arg<0){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}
}

void *multithreading_func(void *arg){
	int ue_socket = *(int*)arg;
	int status;
	char buffer[BUFFER_SIZE];
	const char* response;	
	response = "OK";
	status = read(ue_socket, buffer, sizeof(buffer)-1);
	report_error(status);
	status = write(ue_socket, response, strlen(response));
	report_error(status);
	status = read(ue_socket, buffer, sizeof(buffer)-1);
	report_error(status);
	status = write(ue_socket, response, strlen(response));
	report_error(status);
	cout<<"Registration successful for a client with thread= "<<pthread_self()<<" and sockfd= "<<ue_socket<<endl;
	close(ue_socket);
}

int main() {
	SERVER mme;
	mme.start_listening();
	mme.accept_connections(multithreading_func);
  return 0;
}
