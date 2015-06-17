#include "db_mysql.h"

ConnDetails MySql::details = {"localhost", "root", "mysql", "NFV"};

MySql::MySql(){
	//Dummy constructor
}

void MySql::conn_setup(){
	conn = mysql_init(NULL);;
	if(!mysql_real_connect(conn,details.server,details.user,details.passwd,details.database,0,NULL,0))
		report_error(conn);
	print_message("Connected to database");
}

void MySql::perform_query(const char *query){
	if(mysql_query(conn, query))
		report_error(conn);
	result = mysql_store_result(conn);
}

void MySql::fetch_result(){
	int num_fields;
	MYSQL_ROW row;
	const char *inp;
	num_fields = mysql_num_fields(result);
	print_message("QUERY RESULT");
	while(row = mysql_fetch_row(result)){
		for(int i=0;i<num_fields;i++){
			inp = row[i];
			cout<<inp<<" ";
		}
		cout<<endl;	
	}
}

void MySql::report_error(MYSQL *conn){
	cout<<mysql_error(conn)<<endl;
	exit(EXIT_FAILURE);
}

MySql::~MySql(){
	mysql_free_result(result);
	mysql_close(conn);
}
