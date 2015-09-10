#ifndef DB_MYSQL_H
#define DB_MYSQL_H

#include "utils.h"

//(C) Operations: MySQL_DB functions
#include <mysql/mysql.h>
#include <mysql/my_global.h>

struct ConnDetails {
	char *server;
	char *user;
	char *password;
	char *db;

	ConnDetails();
	ConnDetails(const ConnDetails&);
	friend void swap(ConnDetails&, ConnDetails&);
	ConnDetails& operator=(ConnDetails);
	ConnDetails(ConnDetails&&);
	void set_details();
	~ConnDetails();
};

class MySql {
public:
	static struct ConnDetails conn_details;
	MYSQL *conn;
	MYSQL_RES *result;
	
	MySql();
	MySql(const MySql&);
	friend void swap(MySql&, MySql&);
	MySql& operator=(MySql);
	MySql(MySql&&);
	static void set_conn_details();
	void setup_conn();
	void perform_query(const char*);
	void fetch_result();
	void report_error(MYSQL*);
	~MySql();
};

#endif //DB_MYSQL_H