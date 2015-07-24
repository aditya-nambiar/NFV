#include "sink.h"

int main(){
	Server sink;
	int status;
	struct sockaddr_in client_sock_addr;
	sink.fill_server_details(9190, "127.0.0.1");
	sink.bind_server();
	status = recvfrom(sink.server_socket, sink.pkt.data, BUFFER_SIZE, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
	cout<<"Successfully Received: The message is "<<sink.pkt.data<<endl;
	return 0;
}