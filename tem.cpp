#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <mysql/my_global.h>
using namespace std;

struct ConnDetails {
	const char *server;
	const char *user;
	const char *passwd;
	const char *database;
};

class MySql {
public:
	static ConnDetails details;
	MYSQL *conn;
	MYSQL_RES *result;
	
	MySql();
	void conn_setup();
	void perform_query(const char*);
	void fetch_result();
	void report_error(MYSQL*);
	~MySql();
};

ConnDetails MySql::details = {"localhost", "root", "mysql", "NFV"};

MySql::MySql() {
	
	//Dummy constructor
}

void MySql::conn_setup() {
	
	conn = mysql_init(NULL);;
	if (!mysql_real_connect(conn,details.server,details.user,details.passwd,details.database,0,NULL,0))
		report_error(conn);
}

void MySql::perform_query(const char *query) {
	
	if (mysql_query(conn, query))
		report_error(conn);
	result = mysql_store_result(conn);
}

void MySql::fetch_result() {
	MYSQL_ROW row;
	const char *inp;
	int num_fields;
	int i;

	num_fields = mysql_num_fields(result);
	while (row = mysql_fetch_row(result)) {
		for (i = 0; i < num_fields; i++) {
			inp = row[i];
			cout<<inp<<" ";
		}
		cout<<endl;	
	}
}

void MySql::report_error(MYSQL *conn) {

	cout<<mysql_error(conn)<<endl;
	exit(EXIT_FAILURE);
}

MySql::~MySql() {

	mysql_free_result(result);
	mysql_close(conn);
}

void connect_with_db(MySql &db) {

	db.conn_setup();
}

const char* to_char_array(unsigned long long arg) {
	string tem;
	stringstream out;

	out<<arg;
	tem = out.str();
	const char *ans = tem.c_str();
	return ans;
}

void input_ue_data() {
	string inp_query;
	int i;

	inp_query = "";
	MySql db;
	connect_with_db(db);
	for (i = 101; i <= 1000; i++) {
		inp_query = "";
		inp_query.append("insert into ue_data values(");	
		inp_query.append(to_char_array(i));
		inp_query.append(", ");
		inp_query.append(to_char_array(i*1000));
		inp_query.append(", ");
		inp_query.append(to_char_array(9000000000 + i));
		inp_query.append(")");
		db.perform_query(inp_query.c_str());
	}
}

int main() {	
	
	input_ue_data();	
	return 0;
}