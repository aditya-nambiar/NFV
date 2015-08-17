#include "ran.h"

void setup_tun(){

	system("sudo openvpn --rmtun --dev tun1");
	system("sudo openvpn --mktun --dev tun1");
	system("sudo ip link set tun1 up");
	system("sudo ip addr add 192.168.100.1/24 dev tun1");
}

void* monitor_traffic(void *arg){
	EnodeB enodeb;

	enodeb.attach_to_tun();
	while(g_tun_table.empty());
	while(1){
		enodeb.read_tun();
		enodeb.set_ue_ip();
		enodeb.set_tun_table();
		enodeb.set_sgw_num();
		enodeb.make_data();
		enodeb.send_data();
		enodeb.recv_data();
		enodeb.write_tun();
	}
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
	TunData tun_data;
	EnodeB enodeb;

	ue.authenticate(user);
	tun_data.enodeb_uteid = enodeb.generate_uteid(ue.num)
	ue.setup_tunnel(user, tun_data.enodeb_uteid, tun_data.sgw_uteid, tun_data.sgw_port, tun_data.sgw_addr);
	g_tun_table[ue.ip_addr] = tun_data;
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
	status = pthread_create(&mon_id, NULL, monitor_traffic, NULL);
	report_error(status);
	for(i=0;i<UE_COUNT;i++){
		ue_num[i] = i+1;
		status = pthread_create(&tid[i], NULL, generate_traffic, &ue_num[i]);
		report_error(status);
	}
	for(int i=0;i<UE_COUNT;i++)
		pthread_join(tid[i],NULL);
	return 0;
}
