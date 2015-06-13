#include "utils.h"

void report_error(int arg){
	if(arg<0){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}
}

void report_error(MYSQL *conn){
	cout<<mysql_error(conn)<<endl;
	exit(EXIT_FAILURE);
}

void print_message(string message){
	cout<<"***********************"<<endl;
	cout<<message<<endl;
	cout<<"***********************"<<endl;
}


