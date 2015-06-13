//(C++) Operations: Input/Output, String
#include <iostream>
#include <string>

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

//(C) Operations: MySQL_DB functions
#include <mysql/mysql.h>
#include <mysql/my_global.h>


using namespace std;

#define MAX_CONNECTIONS 100
#define MAX_UE_COUNT 100
#define BUFFER_SIZE 256


void report_error(int);
void report_error(MYSQL*);
void print_message(string);
