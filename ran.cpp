#include "ran.h"

void setup_tun(){
	system("sudo openvpn --rmtun --dev tun1")	;
	system("sudo openvpn --mktun --dev tun1");
	system("sudo ip link set tun1 up");
	system("sudo ip addr add 192.168.100.1/24 dev tun1");
}

void* monitor_traffic(void *arg){
	EnodeB enodeb;

	enodeb.pos = 0;
	enodeb.attach_to_tun();
	while(g_tun_table.empty());
	while(1){
		enodeb.read_tun();
		enodeb.set_ue_ip();
		enodeb.tun_data = g_tun_table[enodeb.ue_ip];
		enodeb.pkt.fill_gtpu_hdr(tun_data.sgw_uteid);
		enodeb.pkt.add_gtpu_hdr();
		enodeb.set_sgw_num();
		enodeb.send_data();
		enodeb.recv_data();
		enodeb.write_tun();
		// Dont know after this
	}
	return NULL;
}

void* generate_traffic(void *arg){
	int ue_num = *(int*)arg;
	Client user;

	user.bind_client();
	user.fill_server_details(g_mme_port, g_mme_addr);
	user.connect_with_server(ue_num);	
	UserEquipment ue(ue_num);
	attach_with_mme(ue, user);
	send_traffic(ue);
}

void attach_with_mme(UserEquipment &ue, Client &user){
	ue.authenticate(user);
	ue.setup_tunnel(user);
}

void send_traffic(UserEquipment &ue){

}

int main(){
	int status;
	vector<int> ue_num(UE_COUNT);
	pthread_t mon_tid;
	pthread_t tid[UE_COUNT];

	setup_tun();
	status = pthread_create(&mon_id, NULL, monitor_traffic, NULL);
	report_error(status);
	for(int i=0;i<UE_COUNT;i++){
		ue_num[i] = i+1;
		status = pthread_create(&tid[i], NULL, generate_traffic, &ue_num[i]);
		report_error(status);
	}
	for(int i=0;i<UE_COUNT;i++)
		pthread_join(tid[i],NULL);
	return 0;
}
