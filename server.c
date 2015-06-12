#include "server.h"

SERVER::SERVER(){
	listen_port = 1234;
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	report_error(listen_socket);
  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(listen_port); 
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  client_addr_len = sizeof(client_addr);	
}

void SERVER::report_error(int arg){
	if(arg<0){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}
}

void SERVER::print_message(const char *message){
	cout<<"***********************"<<endl;
	cout<<message<<endl;
	cout<<"***********************"<<endl;
}

void SERVER::start_listening(){
	int status;
	status = bind(listen_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	report_error(status);
	signal(SIGPIPE, SIG_IGN);
  status = listen(listen_socket, 10);
  report_error(status);
}

void SERVER::accept_connections(){
	int i=0, status;
	while(1){
		client_socket[i] = accept(listen_socket, (struct sockaddr *)&client_addr, &client_addr_len);
		report_error(client_socket[i]);
		print_message("Connected with Client");
		status = pthread_create(&tid[i],NULL,multithreading_func,&listen_socket[i]);
		report_error(status);
		i++;
	}
}

void* multithreading_func(void *arg){
	int new_socket = *(int*)arg;
	print_message("Multithreaded client created");


	close(new_socket);	
}

SERVER::~SERVER(){
	close(listen_socket);
}
