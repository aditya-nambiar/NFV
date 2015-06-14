#include "db.h"

conn_details DB::details = {"localhost", "root", "mysql", "NFV"};

DB::DB(){
	//Dummy constructor
}

void DB::conn_setup(){
	conn = mysql_init(NULL);;
	if(!mysql_real_connect(conn,details.server,details.user,details.passwd,details.database,0,NULL,0))
		report_error(conn);
	print_message("Connected to database");
}

void DB::perform_query(const char *query){
	if(mysql_query(conn, query))
		report_error(conn);
	result = mysql_store_result(conn);
}

void DB::fetch_result(){
	int num_fields;
	MYSQL_ROW row;
	num_fields = mysql_num_fields(result);
	print_message("QUERY RESULT");
	while(row = mysql_fetch_row(result)){
		for(int i=0;i<num_fields;i++)
			cout<<(row[i]?row[i]:"NULL")<<" ";
		cout<<endl;	
	}
}

void DB::report_error(MYSQL *conn){
	cout<<mysql_error(conn)<<endl;
	exit(EXIT_FAILURE);
}

DB::~DB(){
	mysql_free_result(result);
	mysql_close(conn);
}
