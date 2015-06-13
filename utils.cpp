#include "utils.h"

int mme_port = 5000;
int hss_port = 6000;
const char *mme_address = "127.0.0.1";
const char *hss_address = "127.0.0.1";

void report_error(int arg){
	if(arg<0){
		perror("ERROR");
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

void print_message(string message, unsigned long long int arg){
	cout<<"***********************"<<endl;
	cout<<message<<" "<<arg<<endl;
	cout<<"***********************"<<endl;
}
