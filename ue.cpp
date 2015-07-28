#include "ue.h"

TrafficGenerator::TrafficGenerator(){
	data = allocate_str_mem(BUFFER_SIZE);
	src_ip = allocate_str_mem(INET_ADDRSTRLEN);
	dst_ip = allocate_str_mem(INET_ADDRSTRLEN);
}

void TrafficGenerator::fill_traffic(){
	strcpy(data, "This is my traffic");
	strcpy(src_ip, "127.0.0.1");
	strcpy(dst_ip, "127.0.0.1");
	src_port = 60000;
	dst_port = 60001;
}

TrafficGenerator::~TrafficGenerator(){
	free(data);
	free(src_ip);
	free(dst_ip);
}

UserEquipment::UserEquipment(int ue_num){
	key = key_generation(ue_num);
	imsi = key*1000;
	msisdn = 9000000000 + key;
	ip_addr = allocate_str_mem(INET_ADDRSTRLEN);
	set_enodeb_uteid(ue_num);
}

unsigned long long UserEquipment::key_generation(int ue_num){
	return ue_num;
}

void UserEquipment::set_enodeb_uteid(int ue_num){
	enodeb_uteid = ue_num;
}

unsigned long long UserEquipment::get_autn_res(unsigned long long autn, unsigned long long rand){
	unsigned long long res;
	res = autn*key + rand*(key+1);
	return res;
}

void UserEquipment::authenticate(Client &user){
	unsigned long long autn, rand, res;
	char *reply = allocate_str_mem(IP_MAXPACKET);
	type = 1;
	user.pkt.clear_data();
	user.pkt.fill_data(0, sizeof(int), type);
	user.pkt.fill_data(sizeof(int), sizeof(imsi), imsi);
	user.pkt.fill_data(sizeof(int) + sizeof(imsi), sizeof(msisdn), msisdn);
	user.pkt.make_data_packet();
	user.write_data();
	user.read_data();
	memcpy(&autn, user.pkt.data, sizeof(autn));
	memcpy(&rand, user.pkt.data + sizeof(autn), sizeof(rand));
	cout<<autn<<"	"<<rand<<endl;
	res = get_autn_res(autn, rand);
	cout<<"Result is "<<res<<endl;
	user.pkt.clear_data();
	user.pkt.fill_data(0, sizeof(res), res);
	user.pkt.make_data_packet();
	user.write_data();
	user.read_data();
	memcpy(reply, user.pkt.data, user.pkt.data_len);
	cout<<"This is the message -"<<reply<<endl;
	if(strcmp((const char*)reply, "OK") == 0)
		print_message("Authentication Successful for UserEquipment - ", key);
}

void UserEquipment::setup_tunnel(Client &user){
	user.pkt.clear_data();
	user.pkt.fill_data(0, sizeof(uint16_t), enodeb_uteid);
	user.pkt.make_data_packet();
	user.write_data();
	user.read_data();
	memcpy(ip_addr, user.pkt.data, INET_ADDRSTRLEN);
	memcpy(&sgw_uteid, user.pkt.data + INET_ADDRSTRLEN, sizeof(uint16_t));
	user.pkt.fill_gtpu_hdr(sgw_uteid);
	cout<<"Data tunnel is formed from eNodeB to SGW(Both uplink & downlink direction) for UE - "<<key<<endl;
}

void UserEquipment::send_traffic(Client &to_sgw){	
	TrafficGenerator tg;
	int type = 2;
	tg.fill_traffic();
	to_sgw.pkt.clear_data();
	to_sgw.pkt.fill_data(0, sizeof(int), type);
	to_sgw.pkt.make_data_packet();
	to_sgw.write_data();
	to_sgw.pkt.clear_data();
	to_sgw.pkt.fill_data(0, strlen(tg.data), tg.data);
	to_sgw.pkt.fill_ip_hdr(tg.src_ip, tg.dst_ip);
	to_sgw.pkt.fill_udp_hdr(tg.src_port, tg.dst_port);
	to_sgw.pkt.fill_gtpu_hdr(sgw_uteid);	
	to_sgw.pkt.eval_udp_checksum();
	to_sgw.pkt.encap();
	to_sgw.pkt.clear_data();
	to_sgw.pkt.fill_data(0, to_sgw.pkt.packet_len, to_sgw.pkt.packet);
	to_sgw.pkt.add_gtpu_hdr();
	to_sgw.pkt.make_data_packet();
	to_sgw.write_data();
	cout<<"Data sent successfullly"<<endl;
}

UserEquipment::~UserEquipment(){
	// Dummy destructor
}
