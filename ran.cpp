#include "ran.h"

TunData::TunData(){
	sgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

TunData::~TunData(){
	free(sgw_addr);
}

void EnodeB::attach_to_tun(){	
	struct ifreq ifr;
	const char *dev = "tun1";
	const char *clonedev = "/dev/net/tun";
	int flags;

	flags = (IFF_TUN | IFF_NO_PI);
	tun_fd = open(clonedev , O_RDWR);
	report_error(tun_fd, "Opening /dev/net/tun");
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
	if(*dev) {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}
	status = ioctl(tun_fd, TUNSETIFF, (void *)&ifr);
	if(status<0){
		cout<<"ioctl(TUNSETIFF)"<<" "<<errno<<endl;
		close(tun_fd);
		exit(-1);
	}
	strcpy(dev, ifr.ifr_name);
}

void EnodeB::set_ue_ip(){


}

void EnodeB::set_sgw_num(){
	if(socket_table.find(tun_data.sgw_addr) != dic.end())
		num = socket_table[tun_data.sgw_addr];
	else{	
		connect_with_sgw();
		num = socket_table[tun_data.sgw_addr];
	}
}

void EnodeB::connect_with_sgw(){
	to_sgw[pos].bind_client();
	to_sgw[pos].fill_server_details(tun_data.sgw_port, tun_data.sgw_addr);
	to_sgw[pos].connect_with_server(pos);
	socket_table[tun_data.sgw_addr] = pos;
	pos++;
}

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
		enodeb.pkt.clear_data();
		enodeb.count = read(enodeb.tun_fd, enodeb.pkt.data, BUFFER_SIZE);
		enodeb.pkt.data_len = enodeb.count;
		enodeb.set_ue_ip();
		enodeb.tun_data = g_tun_table[ue_ip];
		enodeb.pkt.fill_gtpu_hdr(tun_data.sgw_uteid);
		enodeb.pkt.add_gtpu_hdr();
		enodeb.pkt.make_data_packet();
		enodeb.set_sgw_num();




	}
	// Reads the packet, decide which SGW it has attach to based on the UE IP.	
	// Client to_sgw;
	// to_sgw.bind_client();
	// to_sgw.fill_server_details(g_sgw1_port, g_sgw1_addr);
	// to_sgw.connect_with_server(ue.ue_num);	
	return NULL;
}

void* multithreading_func(void *arg){
	int ue_num = *(int*)arg;
	Client user;

	user.bind_client();
	user.fill_server_details(g_mme_port, g_mme_addr);
	user.connect_with_server(ue_num);	
	UserEquipment ue(ue_num);
	attach_with_mme(ue, user);
	send_traffic(ue, ue_num);
}

void attach_with_mme(UserEquipment &ue, Client &user){
	ue.authenticate(user);
	ue.setup_tunnel(user);
}

void send_traffic(UserEquipment &ue, int &ue_num){

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
		status = pthread_create(&tid[i], NULL, multithreading_func, &ue_num[i]);
		report_error(status);
	}
	for(int i=0;i<UE_COUNT;i++)
		pthread_join(tid[i],NULL);
	return 0;
}
