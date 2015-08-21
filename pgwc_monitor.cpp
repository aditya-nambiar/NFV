#include "pgwc_monitor.h"

PGWcMonitor::PGWcMonitor(){

	tun_name = allocate_str_mem(BUFFER_SIZE);
}

void PGWcMonitor::attach_to_tun(){	
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
	cout<<"Successfully attached to TUN device"<<endl;
}

void PGWcMonitor::read_tun(){

	cout<<"Waiting to read data from the TUN device"<<endl;
	pkt.clear_data();
	count = read(tun_fd, pkt.data, BUFFER_SIZE);
	report_error(count);
	pkt.data_len = count;
	cout<<"Successfully read data from the TUN device"<<endl;
}

void PGWcMonitor::write_tun(){

	count = write(tun_fd, pkt.data, pkt.data_len);
	report_error(count);
	cout<<"Successfully written data into the TUN device"<<endl;
}

void PGWcMonitor::attach_to_sink(){

	to_sink.fill_client_details(g_pgw_addr);
	to_sink.bind_client();
	to_sink.fill_server_details(g_public_sink_port, g_public_sink_addr);
	to_sink.connect_with_server();	
	cout<<"Successfully connected with the Sink server"<<endl;
}

void PGWcMonitor::write_sink(){

	copy_to_sinkpkt();
	to_sink.pkt.make_data_packet();
	to_sink.write_data();		
	cout<<"Successfully written data into the Sink"<<endl;
}

void PGWcMonitor::copy_to_sinkpkt(){
	
	to_sink.pkt.clear_data();
	to_sink.pkt.fill_data(0, pkt.data_len, pkt.data);
}

PGWcMonitor::~PGWcMonitor(){

	free(tun_name);
}
