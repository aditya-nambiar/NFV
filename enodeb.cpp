#include "enodeb.h"

unordered_map<char*, TunData> g_tun_table;

TunData::TunData(){

	sgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

TunData::~TunData(){

	free(sgw_addr);
}

EnodeB::EnodeB(){
	
	socket_table.clear();
	to_sgw.resize(UDP_LINKS);
	pos = 0;
	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
	tun_name = allocate_str_mem(BUFFER_SIZE);

}

uint16_t EnodeB::generate_uteid(int ue_num){
	
	return ue_num; //Dummy uteid
}

void EnodeB::attach_to_tun(){	
	struct ifreq ifr;
	const char *dev = "/dev/net/tun";
	int flags;
	int status;

	strcpy(tun_name, "tun1");
	flags = (IFF_TUN | IFF_NO_PI);
	tun_fd = open(dev , O_RDWR);
	report_error(tun_fd, "Opening /dev/net/tun");
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags;
	if(*tun_name) {
		strncpy(ifr.ifr_name, tun_name, IFNAMSIZ);
	}
	status = ioctl(tun_fd, TUNSETIFF, (void*)&ifr);
	if(status<0){
		cout<<"ioctl(TUNSETIFF)"<<" "<<errno<<endl;
		close(tun_fd);
		exit(-1);
	}
	strcpy(tun_name, ifr.ifr_name);
}

void EnodeB::read_tun(){

	pkt.clear_data();
	count = read(tun_fd, pkt.data, BUFFER_SIZE);
	report_error(count);
	pkt.data_len = count;
}

void EnodeB::write_tun(){

	count = write(tun_fd, pkt.data, pkt.data_len);
	report_error(count);
}

void EnodeB::set_ue_ip(){
	struct ip *iphdr = (ip*)malloc(20 * sizeof(u_int8_t));

	memcpy(iphdr, pkt.data, 20 * sizeof(uint8_t));
	inet_ntop(AF_INET, &(iphdr->ip_src), ue_ip, INET_ADDRSTRLEN);
}

void EnodeB::set_tun_data(){

	tun_data = g_tun_table[ue_ip];
}

void EnodeB::set_sgw_num(){

	if(socket_table.find(tun_data.sgw_addr) != socket_table.end())
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
	handshake_with_sgw();
	socket_table[tun_data.sgw_addr] = pos;
	pos++;
}

void EnodeB::handshake_with_sgw(){
	int type = 2;

	to_sgw[pos].pkt.clear_data();
	to_sgw[pos].pkt.fill_data(0, sizeof(int), type);
	to_sgw[pos].pkt.make_data_packet();
	to_sgw[pos].write_data();	
}

void EnodeB::make_data(){

	pkt.fill_gtpu_hdr(tun_data.sgw_uteid);
	pkt.add_gtpu_hdr();
}

void EnodeB::send_data(){

	to_sgw[num].pkt.clear_data();
	to_sgw[num].pkt.fill_data(0, pkt.data_len, pkt.data);
	to_sgw[num].pkt.make_data_packet();
	to_sgw[num].write_data();
}

void EnodeB::recv_data(){

	to_sgw[num].read_data();
	pkt.clear_data();	
	pkt.fill_data(0, to_sgw[num].pkt.data_len, to_sgw[num].pkt.data);
	pkt.rem_gtpu_hdr();
}

EnodeB::~EnodeB(){

	free(ue_ip);
	free(tun_name);
}