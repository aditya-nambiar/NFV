#include "hss.h"

const char *query = "SELECT * FROM UE";

void* multithreading_func(void *arg){
	int mme_socket = *(int*)arg;
	DB db;
	connect_with_db(db);
	fetch_db_data(db, query);
	close(mme_socket);	
}

void connect_with_db(DB &db){
	db.conn_setup();
}

void fetch_db_data(DB &db, const char *query){
	db.perform_query(query);
	db.fetch_result();
}

int main(){
	SERVER hss(hss_port, hss_address);
	hss.start_listening();
	hss.accept_connections(multithreading_func);
	return 0;
}
