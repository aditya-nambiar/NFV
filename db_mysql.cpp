#include "db_mysql.h"

struct ConnDetails MySql::conn_details;

ConnDetails::ConnDetails() {
	
	server = allocate_str_mem(BUFFER_SIZE);
	user = allocate_str_mem(BUFFER_SIZE);
	password = allocate_str_mem(BUFFER_SIZE);
	db = allocate_str_mem(BUFFER_SIZE);
}

ConnDetails::ConnDetails(const ConnDetails &src_obj) {

	server = allocate_str_mem(BUFFER_SIZE);
	user = allocate_str_mem(BUFFER_SIZE);
	password = allocate_str_mem(BUFFER_SIZE);
	db = allocate_str_mem(BUFFER_SIZE);
	strcpy(server, src_obj.server);
	strcpy(user, src_obj.user);
	strcpy(password, src_obj.password);
	strcpy(db, src_obj.db);
}

void swap(ConnDetails &src_obj, ConnDetails &dst_obj) {
	using std::swap;

	swap(src_obj.server, dst_obj.server);
	swap(src_obj.user, dst_obj.user);
	swap(src_obj.password, dst_obj.password);
	swap(src_obj.db, dst_obj.db);
}

ConnDetails& ConnDetails::operator=(ConnDetails src_obj) {

	swap(*this, src_obj);
	return *this;
}

ConnDetails::ConnDetails(ConnDetails &&src_obj)
	:ConnDetails() {

	swap(*this, src_obj);
}

void ConnDetails::set_details() {

	strcpy(server, "localhost");
	strcpy(user, "root");
	strcpy(password, "mysql");
	strcpy(db, "NFV");
}

ConnDetails::~ConnDetails() {

	free(server);
	free(user);
	free(password);
	free(db);
}

MySql::MySql() {

	conn = NULL;
	result = NULL;
}

MySql::MySql(const MySql &src_obj) {

	conn = src_obj.conn;
	result = src_obj.result;
}

void swap(MySql &src_obj, MySql &dst_obj) {
	using std::swap;

	swap(src_obj.conn, dst_obj.conn);
	swap(src_obj.result, dst_obj.result);
}

MySql& MySql::operator=(MySql src_obj) {

	swap(*this, src_obj);
	return *this;
}

MySql::MySql(MySql &&src_obj)
	:MySql() {

	swap(*this, src_obj);
}

void MySql::set_conn_details() {

	conn_details.set_details();
}

void MySql::setup_conn() {
	
	conn = mysql_init(NULL);
	if (!mysql_real_connect(conn, conn_details.server, conn_details.user, conn_details.password, conn_details.db, 0, NULL, 0))
		report_error(conn);
	print_message("Connected to database");
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
	print_message("QUERY RESULT");
	while (row = mysql_fetch_row(result)) {
		for (i = 0; i < num_fields; i++) {
			inp = row[i];
			cout << inp << " ";
		}
		cout << endl;	
	}
}

void MySql::report_error(MYSQL *conn) {
	
	cout << mysql_error(conn) << endl;
	exit(EXIT_FAILURE);
}

MySql::~MySql() {

	mysql_free_result(result);
	mysql_close(conn);
}
