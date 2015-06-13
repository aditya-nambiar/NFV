//(C++) Operations: Input/Output
#include <iostream>

//(C++) STL Operations: String, Vector
#include <string>
#include <vector>

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

#define MAX_CONNECTIONS 1
#define MAX_UE_COUNT 1
#define BUFFER_SIZE 256

extern int mme_port;
extern int hss_port;
extern const char *mme_address;
extern const char *hss_address;

void report_error(int);
void print_message(string);
void print_message(string,int);
void print_message(string,unsigned long long int);
