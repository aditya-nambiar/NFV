#include "pgw.h"

void Tunnel::set_pgw_cteid(int ue_num){
	pgw_cteid =  ue_num;
}

void Tunnel::set_pgw_uteid(int ue_num){
	pgw_uteid =  ue_num;
}

void set_static_ips(){
	int i;
	for(i=0;i<MAX_CONNECTIONS;i++){
		g_static_ips[i] = allocate_str_mem(INET_ADDRSTRLEN);
		strcpy(g_static_ips[i], UE_IP_ADDR);
	}
}

void* multithreading_func(void *arg){
	int type;
	ClientDetails entity = *(ClientDetails*)arg;
	Server pgw;
	Tunnel tun;
	tun.set_pgw_cteid(entity.num);
	tun.set_pgw_uteid(entity.num);
	pgw.fill_server_details(g_freeport, g_pgw_addr);
	pgw.bind_server();
	pgw.client_sock_addr = entity.client_sock_addr;
	pgw.connect_with_client();
	pgw.read_data();
	memcpy(&type, pgw.pkt.data, sizeof(int));
	if(type == 1)
		handle_cdata(pgw, tun, entity.num);
	else if(type == 2)
		handle_udata(pgw, tun, entity.num);
}

void handle_cdata(Server &pgw, Tunnel &tun, int &ue_num){
	int user_num;
	int bearer_id;
	memcpy(&user_num, pgw.pkt.data + sizeof(int), sizeof(int));
	memcpy(&bearer_id, pgw.pkt.data + 2*sizeof(int), sizeof(int));
	memcpy(&tun.sgw_cteid, pgw.pkt.data + 3*sizeof(int), sizeof(uint16_t));
	memcpy(&tun.sgw_uteid, pgw.pkt.data + 3*sizeof(int) + sizeof(uint16_t), sizeof(uint16_t));
	pgw.pkt.clear_data();
	set_bearer_id(ue_num, bearer_id);
	pgw.pkt.fill_data(0, sizeof(uint16_t), tun.pgw_cteid);
	pgw.pkt.fill_data(sizeof(uint16_t), sizeof(uint16_t), tun.pgw_uteid);
	pgw.pkt.fill_data(2*sizeof(uint16_t), INET_ADDRSTRLEN, g_static_ips[ue_num]);
	pgw.pkt.fill_gtpc_hdr(tun.sgw_cteid);
	pgw.pkt.add_gtpu_hdr();
	pgw.pkt.make_data_packet();
	pgw.write_data();
	cout<<"Tunnel created from PGW to SGW for UE - "<<ue_num<<endl;
}

void handle_udata(Server &sgw, Tunnel &tun, int &ue_num){

}

void set_bearer_id(int ue_num, int bearer_id){
	g_bearer_table[ue_num] = bearer_id;
}

void free_static_ips(){
	int i;
	for(i=0;i<MAX_CONNECTIONS;i++){
		free(g_static_ips[i]);
	}
}

int main(){
	set_static_ips();
	Server pgw;
	pgw.fill_server_details(g_pgw_port, g_pgw_addr);
	pgw.bind_server();
	pgw.listen_accept(multithreading_func);
	free_static_ips();
	return 0;
}