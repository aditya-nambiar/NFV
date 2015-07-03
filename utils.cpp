#include "utils.h"

int g_mme_port = 5000;
int g_hss_port = 9000;
const char *g_mme_address = "127.0.0.1";
const char *g_hss_address = "127.0.0.1";
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
