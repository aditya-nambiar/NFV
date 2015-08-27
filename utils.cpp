#include "utils.h"

int g_freeport = 0;
int g_mme_port = 4000;
int g_hss_port = 5000;
int g_sgw1_port = 6000;
int g_sgw2_port = 7000;
int g_sgw3_port = 8000;
int g_pgw_port = 9000;
int g_public_sink_port = 5000;
int g_private_sink_port = 5000;
int g_pgw_server_for_sink_port = 6000;
const char *g_mme_addr = "10.129.26.169";
const char *g_hss_addr = "10.129.26.169";
const char *g_sgw1_addr = "10.129.26.169";
const char *g_sgw2_addr = "10.129.26.169";
const char *g_sgw3_addr = "10.129.26.169";
const char *g_pgw_addr = "10.14.13.29";
const char *g_public_sink_addr = "10.129.45.90";
const char *g_private_sink_addr = "192.168.100.2";
const char *g_pgw_server_for_sink_addr = "10.14.13.29";
socklen_t g_addr_len = sizeof(sockaddr_in);
timeval g_timeout = {2, 0};

void check_conn(int &status){

	if(errno==EAGAIN || errno==EWOULDBLOCK || errno==EINPROGRESS)
		status = 1000;
}

void report_error(int arg){

	if(arg<0){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}
}

void report_error(int arg, const char *message){

	if(arg<0){
		perror(message);
		exit(EXIT_FAILURE);
	}
}

void print(string message){

	cout<<message<<endl;
}

void print(int arg){

	cout<<arg<<endl;
}

void print(string message, int arg){

	cout<<message<<" "<<arg<<endl;
}

void print_message(string message){

	cout<<"***********************"<<endl;
	cout<<message<<endl;
	cout<<"***********************"<<endl;
}

void print_message(string message, int arg){

	cout<<"***********************"<<endl;
	cout<<message<<" "<<arg<<endl;
	cout<<"***********************"<<endl;
}

void print_message(string message, unsigned long long arg){

	cout<<"***********************"<<endl;
	cout<<message<<" "<<arg<<endl;
	cout<<"***********************"<<endl;
}

const char* to_char_array(unsigned long long arg){
	string tem;
	stringstream out;

	out<<arg;
	tem = out.str();
	const char *ans = tem.c_str();
	return ans;
}

char* allocate_str_mem(int len){
	char *tem;

	if(len <= 0){
		print("ERROR: Given_memory_length<=0");
		exit (EXIT_FAILURE);
	}
	tem = (char*)malloc(len * sizeof (char));
	if(tem != NULL){
		memset(tem, 0, len * sizeof (char));
		return tem;
	}
	else{
		print("ERROR: Memory allocation failure");
		exit(EXIT_FAILURE);
	}
}

uint8_t* allocate_uint8_mem(int len){
	uint8_t *tem;

	if(len <= 0){
		print("ERROR: Given_memory_length<=0");
		exit(EXIT_FAILURE);
	}
	tem = (uint8_t*)malloc(len * sizeof (uint8_t));
	if(tem != NULL){
		memset(tem, 0, len * sizeof (uint8_t));
		return tem;
	} 
	else{
		print("ERROR: Memory allocation failure");
		exit (EXIT_FAILURE);
	}
}

void usage(int argc, char *argv[]){

	if(argc < 2){
		cout<<"Please try again with the number of threads to be spawn"<<endl;
		exit(-1);
	}
	if(atoi(argv[1]) == 0){
		cout<<"Please try again with a valid number of threads to be spawn"<<endl;
		exit(-1);
	}
}

