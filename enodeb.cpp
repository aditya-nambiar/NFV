#include "enodeb.h"

TunData::TunData(){
	sgw_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

TunData::~TunData(){
	free(sgw_addr);
}

EnodeB::EnodeB(){
	ue_ip = allocate_str_mem(INET_ADDRSTRLEN);
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

void EnodeB::send_data(){
	to_sgw[num].pkt.clear_data();
	to_sgw[num].pkt.add_data(0, pkt.data_len, pkt.data);
	to_sgw[num].pkt.make_data_packet();
	to_sgw[num].pkt.write_data();
}

void EnodeB::recv_data(){
	to_sgw[num].pkt.read_data();
	pkt.clear_data();	
	pkt.add_data(0, to_sgw[num].pkt.data_len, to_sgw[num].data);
	pkt.rem_gtpu_hdr();
}

EnodeB::~EnodeB(){
	free(ue_ip);
}
