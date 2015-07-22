#ifndef UTILS_H
#define UTILS_H

//(C++) Operations: Input/Output
#include <iostream>

//(C++) STL Operations: String, Vector, String stream, Unordered map
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

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

#define MAX_CONNECTIONS 1000
#define UE_COUNT 100
#define BUFFER_SIZE 1024
#define UE_IP_ADDR "127.0.0.1"

extern int g_freeport;
extern int g_mme_port;
extern int g_hss_port;
extern int g_sgw1_port;
extern int g_sgw2_port;
extern int g_sgw3_port;
extern int g_pgw_port;
extern const char *g_mme_addr;
extern const char *g_hss_addr;
extern const char *g_sgw1_addr;
extern const char *g_sgw2_addr;
extern const char *g_sgw3_addr;
extern const char *g_pgw_addr;
extern socklen_t g_addr_len;
extern timeval g_timeout;
extern long long g_stack_buf;

struct ClientDetails{
	int num;
	struct sockaddr_in client_sock_addr;
};

void check_conn(int&);
void report_error(int);
void report_error(int, const char*);
void print(string);
void print(int);
void print(string, int);
void print_message(string);
void print_message(string, int);
void print_message(string, unsigned long long);
const char* to_char_array(unsigned long long);
char* allocate_str_mem(int);
uint8_t* allocate_uint8_mem(int);

#endif //UTILS_H