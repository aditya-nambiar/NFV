#include "ran.h"

void setup_tun(){

	system("sudo openvpn --rmtun --dev tun1");
	system("sudo openvpn --mktun --dev tun1");
	system("sudo ip link set tun1 up");
	system("sudo ip addr add 192.168.100.1/24 dev tun1");
}

void* monitor_traffic(void *arg){
	EnodeB enodeb;
	fd_set read_set;
	int max_fd;
	int size;
	int i;
	int status;

	enodeb.attach_to_tun();
	// while(g_tun_table.empty());
	while(1){
		FD_ZERO(&read_set);
		FD_SET(enodeb.tun_fd, &read_set); 
		max_fd = enodeb.tun_fd;
		size = enodeb.pos;
		for(i=0;i<size;i++){
			FD_SET(enodeb.to_sgw[i].client_socket, &read_set); 
			max_fd = max(max_fd, enodeb.to_sgw[i].client_socket);
		}
		status = select(max_fd + 1, &read_set, NULL, NULL, NULL);
		report_error(status, "Select-process failure\tTry again");		
		if(FD_ISSET(enodeb.tun_fd, &read_set)){
			enodeb.read_tun();
			enodeb.set_ue_ip();
			enodeb.set_tun_data();
			enodeb.set_sgw_num();
			enodeb.make_data();
			enodeb.send_data();
		}
		for(i=0;i<size;i++){
			if(FD_ISSET(enodeb.to_sgw[i].client_socket, &read_set)){
				enodeb.recv_data(i);
				enodeb.write_tun();
			}
		}
	}
}

void* generate_traffic(void *arg){
	int ue_num = *(int*)arg;
	Client to_mme;

	to_mme.bind_client();
	to_mme.fill_server_details(g_mme_port, g_mme_addr);
	to_mme.connect_with_server(ue_num);	
	UserEquipment ue(ue_num);
	attach_with_mme(ue, to_mme);
	send_traffic(ue);
}

void attach_with_mme(UserEquipment &ue, Client &to_mme){
	EnodeB enodeb;
	TunData tun_data;
	uint16_t enodeb_uteid;
	string ue_ip_str;

	ue.authenticate(to_mme);
	enodeb_uteid = enodeb.generate_uteid(ue.num);
	ue.setup_tunnel(to_mme, enodeb_uteid, tun_data.sgw_uteid, tun_data.sgw_port, tun_data.sgw_addr);
	ue_ip_str.assign(ue.ip_addr);
	g_tun_table[ue_ip_str] = tun_data;
}

void send_traffic(UserEquipment &ue){

	ue.send_traffic();
}

int main(){
	int status;
	int i;
	vector<int> ue_num(UE_COUNT);
	pthread_t mon_tid;
	pthread_t tid[UE_COUNT];

	setup_tun();
	status = pthread_create(&mon_tid, NULL, monitor_traffic, NULL);
	report_error(status);
	for(i=0;i<UE_COUNT;i++){
		ue_num[i] = i;
		status = pthread_create(&tid[i], NULL, generate_traffic, &ue_num[i]);
		report_error(status);
	}
	// for(int i=0;i<UE_COUNT;i++)
	// 	pthread_join(tid[i],NULL);
	pthread_join(mon_tid, NULL);
	return 0;
}
