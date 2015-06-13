#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

//(C) Operations: MySQL_DB functions
#include <mysql/mysql.h>
#include <mysql/my_global.h>


struct conn_details{
	const char *server;
	const char *user;
	const char *passwd;
	const char *database;
};

struct sql_data{
	MYSQL *conn;
	MYSQL_RES *result;
};

class DB{
	public:
		static conn_details details;
		sql_data sql;
	
		DB();
		void conn_setup();
		void perform_query(const char*);
		void fetch_result();
		void report_error(MYSQL*);
		~DB();
};
