#include "ran.h"

void setup_tun(){
	system("sudo openvpn --rmtun --dev tun1")	;
	system("sudo openvpn --mktun --dev tun1");
	system("sudo ip link set tun1 up");
	system("sudo ip addr add 10.129.26.1/24 dev tun1");
}

int tun_alloc(char *dev, int flags){
	struct ifreq ifr;
	int fd, err;
	const char *clonedev = "/dev/net/tun";
	if((fd = open(clonedev , O_RDWR)) < 0){
		fprintf(stderr,"Opening /dev/net/tun");
		exit(-1);
	}
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
	if (*dev) {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}
	if((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0){
		fprintf(stderr,"ioctl(TUNSETIFF)\n");
		printf("%d\n", errno);
		close(fd);
		exit(-1);
	}
	strcpy(dev, ifr.ifr_name);
	return fd;
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
	int status;
	pthread_t mon_id;
	status = pthread_create(&mon_id, NULL, monitor_traffic, &ue);
	monitor_traffic(ue, ue_num);
	ue.send_traffic(to_sgw);
}

void* monitor_traffic(void *arg){
	int status;
	int tun_fd;
	UserEquipment ue = *(*ue)arg;
	tun_fd = attach_to_tun();
	Client to_sgw;
	to_sgw.bind_client();
	to_sgw.fill_server_details(g_sgw1_port, g_sgw1_addr);
	to_sgw.connect_with_server(ue.ue_num);	



}

int attach_to_tun(){
	int flags = IFF_TUN;
	char if_name[IFNAMSIZ] = "tun1";
	return tun_alloc(if_name, flags | IFF_NO_PI);
}

void receive_reply(UserEquipment &ue){

}

int main(){
	int status;
	vector<int> ue_num(UE_COUNT);
	pthread_t tid[UE_COUNT];
	setup_tun();
	for(int i=0;i<UE_COUNT;i++){
		ue_num[i] = i+1;
		status = pthread_create(&tid[i], NULL, multithreading_func, &ue_num[i]);
		report_error(status);
	}
	for(int i=0;i<UE_COUNT;i++)
		pthread_join(tid[i],NULL);
	return 0;
}
