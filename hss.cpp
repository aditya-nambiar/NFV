#include "server.h"
#include "db.h"

void fetch_DBdata(const char *query){
	DB db;
	db.conn_setup();
	db.perform_query(query);
	db.fetch_result();
}

void* multithreading_func(void *arg){
	int new_socket = *(int*)arg;


	close(new_socket);	
}

int main(){
	SERVER hss;
	hss.start_listening();
	hss.accept_connections(multithreading_func);
	return 0;
}
