#include <mysql/mysql.h>
#include <mysql/my_global.h>
using namespace std;

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
		conn_details details;
		sql_data sql;
	
		DB();
		void conn_setup();
		void perform_query(const char*);
		void fetch_result();
		void print_message(const char*);
		~DB();
};
