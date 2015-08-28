#include "ue.h"

UserEquipment::UserEquipment(int ue_num){

	num = ue_num;
	key = generate_key(num);
	imsi = key*1000;
	msisdn = 9000000000 + key;
	
	cout<<"IMSI is "<<imsi<<" "<<"MSISDN is "<<msisdn<<endl;
	ip_addr = allocate_str_mem(INET_ADDRSTRLEN);
	sink_addr = allocate_str_mem(INET_ADDRSTRLEN);
}

unsigned long long UserEquipment::generate_key(int ue_num){

	return ue_num;
}

void UserEquipment::authenticate(Client &to_mme){
	unsigned long long autn, rand, res;
	char *reply = allocate_str_mem(IP_MAXPACKET);

	type = 1;
	to_mme.pkt.clear_data();
	to_mme.pkt.fill_data(0, sizeof(int), type);
	to_mme.pkt.fill_data(sizeof(int), sizeof(imsi), imsi);
	to_mme.pkt.fill_data(sizeof(int) + sizeof(imsi), sizeof(msisdn), msisdn);
	to_mme.pkt.make_data_packet();
	to_mme.write_data();
	to_mme.read_data();
	memcpy(&autn, to_mme.pkt.data, sizeof(autn));
	memcpy(&rand, to_mme.pkt.data + sizeof(autn), sizeof(rand));
	res = get_autn_res(autn, rand);
	cout<<"AUTN is "<<autn<<", RAND is "<<rand<<" and the result is "<<res<<endl;
	to_mme.pkt.clear_data();
	to_mme.pkt.fill_data(0, sizeof(res), res);
	to_mme.pkt.make_data_packet();
	to_mme.write_data();
	to_mme.read_data();
	memcpy(reply, to_mme.pkt.data, to_mme.pkt.data_len);
	cout<<"This is the message -"<<reply<<endl;
	if(strcmp((const char*)reply, "OK") == 0)
		print_message("Authentication Successful for UserEquipment - ", key);
}

unsigned long long UserEquipment::get_autn_res(unsigned long long autn, unsigned long long rand){
	unsigned long long res;

	res = autn*key + rand*(key+1);
	return res;
}

void UserEquipment::setup_tunnel(Client &to_mme, uint16_t &enodeb_uteid, uint16_t &sgw_uteid, int &sgw_port, string &sgw_addr){

	to_mme.pkt.clear_data();
	to_mme.pkt.fill_data(0, sizeof(uint16_t), enodeb_uteid);
	to_mme.pkt.make_data_packet();
	to_mme.write_data();
	to_mme.read_data();
	memcpy(&sgw_uteid, to_mme.pkt.data, sizeof(uint16_t));
	memcpy(ip_addr, to_mme.pkt.data + sizeof(uint16_t), INET_ADDRSTRLEN);
	sgw_port = g_sgw1_port;
	sgw_addr.assign(g_sgw1_addr);
	cout<<"Data tunnel is formed from eNodeB to SGW(Both uplink & downlink direction) for UE - "<<key<<endl;
}

void UserEquipment::send_traffic(){	
	string command;
	string ip_addr_str;
	string sink_addr_str;
	string rate;
	string mtu;
	string time_limit;

	setup_interface();
	set_sink();
	ip_addr_str.assign(ip_addr);
	sink_addr_str.assign(sink_addr);
	rate = " -b 200M";
	mtu = " -M 500";
	time_limit = " -t 1";
	command = "iperf3 -B " + ip_addr_str + " -c " + sink_addr + " -p " + to_string(sink_port) + rate + mtu + time_limit; 
	cout<<command<<endl;
	system(command.c_str());
	cout<<"IPERF Traffic successfully sent"<<endl;
	// generate_data();
	// to_sink.fill_client_details(ip_addr);
	// to_sink.bind_client();
	// to_sink.fill_server_details(sink_port, sink_addr);
	// to_sink.connect_with_server();
	// to_sink.pkt.clear_data();
	// to_sink.pkt.fill_data(0, pkt.data_len, pkt.data);
	// to_sink.pkt.make_data_packet();
	// to_sink.write_data();	
	// cout<<"Data sent successfully for UE - "<<num<<endl;
}

void UserEquipment::setup_interface(){
	string arg;

	arg = "sudo ifconfig eth0:";
	arg.append(to_string(num));
	arg.append(" ");
	arg.append(ip_addr);
	arg.append("/16");
	cout<<arg.c_str()<<endl;
	system(arg.c_str());
	cout<<"Interface successfully created for UE - "<<num<<endl;
}

void UserEquipment::set_sink(){

	// sink_port = g_private_sink_port;
	sink_port = (num + 55000);
	strcpy(sink_addr, g_private_sink_addr);
}

void UserEquipment::generate_data(){

	pkt.clear_data();
	pkt.fill_data(0, 19, "This is my traffic");
}

void UserEquipment::send_detach_req(Client &to_mme){
	int type = 3;

	to_mme.pkt.clear_data();
	to_mme.pkt.fill_data(0, sizeof(int), type);
	to_mme.pkt.make_data_packet();
	to_mme.write_data();
}

void UserEquipment::recv_detach_res(Client &to_mme){
	char *reply;

	reply = allocate_str_mem(BUFFER_SIZE);
	to_mme.read_data();
	memcpy(reply, to_mme.pkt.data, to_mme.pkt.data_len);
	if(strcmp((const char*)reply, "OK") == 0){
		cout<<"UE - "<<num<<" has successfully detached from EPC"<<endl;
	}
	free(reply);
}

UserEquipment::~UserEquipment(){

	free(ip_addr);
	free(sink_addr);
}
