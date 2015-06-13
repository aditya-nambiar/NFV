#include "mme.h"

void *multithreading_func(void *arg){
	int ue_socket = *(int*)arg;
	close(ue_socket);
}

int main(){
	SERVER mme(mme_port, mme_address);
	mme.start_listening();
	mme.accept_connections(multithreading_func);
  return 0;
}
