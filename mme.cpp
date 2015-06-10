#include <iostream>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>

#include <pthread.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
using namespace std;

#define IMSI_LENGTH 8
#define BUFFER_SIZE 256
#define MAX_UE_COUNT 100

void handleErrors(void);
char* read_mme(int sock, unsigned char* buffer, int read_size);
int write_mme(int sock, unsigned char* write_data, int size_data);
void *multithreading_func(void *arg);
static void usage();

int main(int argc, char *argv[]) {
  int err, s0, res, listenPort;
  int s1[MAX_UE_COUNT];
  pthread_t tid;
  struct sockaddr_in myaddr, peeraddr;
  socklen_t peeraddr_len; 
  if(argc > 1 && *(argv[1]) == '-'){
    usage(); 
    exit(1);
  }
  listenPort = 1234;  
  if (argc > 1)
    listenPort = atoi(argv[1]);
  s0 = socket(AF_INET, SOCK_STREAM, 0);
  if(s0 < 0){
    perror("Cannot create a socket"); 
    exit(1);
  }
  bzero((char *) &myaddr, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = htons(listenPort); 
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  res = bind(s0, (struct sockaddr*) &myaddr, sizeof(myaddr));
  if(res < 0){
    perror("Cannot bind a socket"); 
    exit(1);
  }
  
  struct linger linger_opt = { 1, 0 }; 
  setsockopt(s0, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));
	
	signal(SIGPIPE, SIG_IGN);
	
  res = listen(s0, 10);
  if (res < 0) {
    perror("Cannot listen"); 
    exit(1);
  }
  
	peeraddr_len = sizeof(peeraddr);	
	int i=0;
	while(1){
		s1[i] = accept(s0, (struct sockaddr *) &peeraddr, &peeraddr_len);
		if(s1<0){
			cout<<"Client Connection Accept Problem - Socket not created\nTry Again\n"; 	
		}
		cout<<"Connected with client"<<endl;
		err = pthread_create(&tid,NULL,multithreading_func,&s1[i]);
		if(err<0){
			cout<<"Error in Thread Creation\n";
		}
		i++;
	}
  return 0;
}

void *multithreading_func(void *arg){
	int n, res;
	int s1 = *(int*)arg;
	char buffer[BUFFER_SIZE];
	char *request;
	char *response = "OK";
	cout<<"Server-"<<pthread_self()<<" Sockfd is "<<s1<<endl;
	/*request = read_mme(s1, (unsigned char*)buffer, BUFFER_SIZE-1);
	if(write_mme(s1, (unsigned char*)response, sizeof(response)-1) == 0){
		perror("cannot write on socket!");
	}
	request = read_mme(s1, (unsigned char*)buffer, BUFFER_SIZE-1);
	if(write_mme(s1, (unsigned char*)response, sizeof(response)-1) == 0){
		perror("cannot write on socket!");
	}	
	cout<<"Registration successful for a client"<<endl;
	close(s1);*/
	read(s1, buffer, BUFFER_SIZE-1);
	if(write(s1, response, strlen(response)) < 0){
		perror("cannot write on socket!");
	}else{
		read(s1, buffer, BUFFER_SIZE-1);
		if(write(s1, response, sizeof(response)-1) < 0){
			perror("cannot write on socket!");
		}else
			cout<<"Registration successful for a client with thread "<<pthread_self()<<" and sock is"<<s1<<endl;
	}
	close(s1);
}

void handleErrors(void){
  abort();
}

char* read_mme(int sock, unsigned char* buffer, int read_size){
	int res;
  memset(buffer, 0, sizeof(buffer));
  res = read(sock, buffer, read_size);
  cout<<res<<endl;
  if (res < 0) {
    perror("Read error");
    return 0;
  }
  cout<<"Received packet "<<buffer<<endl;
  return (char*)buffer;
}

int write_mme(int sock, unsigned char* write_data, int size_data){
	int res;
	res = write(sock, write_data, size_data);
	if(res<0){
		perror("Error writing data");
		return 0;
	}
	return 1;
}

static void usage() {
  printf(
	 "A simple Internet server application.\n"
	 "It listens to the port written in command line (default 1234),\n"
	 "accepts a connection, and sends the \"Hello!\" message to a client.\n"
	 "Then it receives the answer from a client and terminates.\n\n"
	 "Usage:\n"
	 "     server [port_to_listen]\n"
	 "Default is the port 1234.\n"
	 );
}
