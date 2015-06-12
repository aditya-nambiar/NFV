#include <iostream>
#include "hss.h"

DB::DB(){
	details.server = "localhost";
	details.user = "root";
	details.passwd = "mysql";
	details.database = "NFV";
}

void DB::print_message(const char *message){
	cout<<"***********************"<<endl;
	cout<<message<<endl;
	cout<<"***********************"<<endl;
}

void DB::conn_setup(){
	sql.conn = mysql_init(NULL);;
	if(!mysql_real_connect(sql.conn,details.server,details.user,details.passwd,details.database,0,NULL,0)){
		cout<<mysql_error(sql.conn)<<endl;
		exit(1);
	}
	print_message("Connected to database");
}

void DB::perform_query(const char *query){
	if(mysql_query(sql.conn, query)){
		cout<<mysql_error(sql.conn)<<endl;
		exit(1);
	}
	sql.result = mysql_store_result(sql.conn);
}

void DB::fetch_result(){
	int num_fields;
	MYSQL_ROW row;
	num_fields = mysql_num_fields(sql.result);
	print_message("QUERY RESULT");
	while(row = mysql_fetch_row(sql.result)){
		for(int i=0;i<num_fields;i++)
			cout<<(row[i]?row[i]:"NULL")<<" ";
		cout<<endl;	
	}
}

DB::~DB(){
	mysql_free_result(sql.result);
	mysql_close(sql.conn);
}
