#include "sink.h"

int main(){
	Server sink;
	int status;
	struct sockaddr_in client_sock_addr;
	sink.fill_server_details(60001, "127.0.0.1");
	sink.bind_server();
	cout<<sink.server_port<<" "<<sink.server_addr<<endl;
	status = recvfrom(sink.server_socket, sink.pkt.data, BUFFER_SIZE, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
	cout<<ntohs(client_sock_addr.sin_port)<<endl;
	cout<<"Successfully Received: "<<status<<" bytes"<<"and the message is "<<sink.pkt.data<<endl;
	return 0;
}