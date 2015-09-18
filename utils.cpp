#include "utils.h"

int g_reuse = 1;
int g_freeport = 0;
int g_mme_port = 4000;
int g_hss_port = 5000;
int g_sgw1_port = 6000;
int g_sgw2_port = 7000;
int g_sgw3_port = 8000;
int g_pgw_port = 9000;
int g_public_sink_port = 5000;
int g_private_sink_port = 5000;
int g_pgw_server_for_sink_port = 6000;

// const char *g_mme_addr = "10.129.26.169";
// const char *g_hss_addr = "10.129.26.169";
// const char *g_sgw1_addr = "10.129.26.169";
// const char *g_sgw2_addr = "10.129.26.169";
// const char *g_sgw3_addr = "10.129.26.169";
// const char *g_pgw_addr = "10.14.13.29";
// const char *g_pgw_server_for_sink_addr = "10.14.13.29";
// const char *g_public_sink_addr = "10.129.45.90";
// const char *g_private_sink_addr = "192.168.100.2";

const char *g_mme_addr = "192.168.1.70";
const char *g_hss_addr = "192.168.1.70";
const char *g_sgw1_addr = "192.168.1.70";
const char *g_sgw2_addr = "192.168.1.70";
const char *g_sgw3_addr = "192.168.1.70";
const char *g_pgw_addr = "192.168.1.76";
const char *g_pgw_server_for_sink_addr = "192.168.1.76";
const char *g_public_sink_addr = "192.168.1.78";
const char *g_private_sink_addr = "192.168.100.2";

socklen_t g_addr_len = sizeof(sockaddr_in);
timeval g_timeout = {2, 0};

ClientDetails::ClientDetails() {


}

ClientDetails::ClientDetails(const ClientDetails &src_obj) {

	num = src_obj.num;
	client_sock_addr = src_obj.client_sock_addr;
}

void swap(ClientDetails &src_obj, ClientDetails &dst_obj) {
	using std::swap;

	swap(src_obj.num, dst_obj.num);
	swap(src_obj.client_sock_addr, dst_obj.client_sock_addr);
}

ClientDetails& ClientDetails::operator=(ClientDetails src_obj) {

	swap(*this, src_obj);
	return *this;
}

ClientDetails::ClientDetails(ClientDetails &&src_obj)
	:ClientDetails() {

	swap(*this, src_obj);
}

ClientDetails::~ClientDetails() {


}

void check_conn(int &status) {

	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINPROGRESS)
		status = 1000;
}

void report_error(int arg) {

	if (arg < 0) {
		perror("ERROR");
		exit(EXIT_FAILURE);
	}
}

void report_error(int arg, const char *message) {

	if (arg < 0) {
		perror(message);
		exit(EXIT_FAILURE);
	}
}

void print(string message) {

	cout << message << endl;
}

void print(int arg) {

	cout << arg << endl;
}

void print(string message, int arg) {

	cout << message << " " << arg << endl;
}

void print_message(string message) {

	cout << "***********************" << endl;
	cout << message << endl;
	cout << "***********************" << endl;
}

void print_message(string message, int arg) {

	cout << "***********************" << endl;
	cout << message << " " << arg << endl;
	cout << "***********************" << endl;
}

void print_message(string message, unsigned long long arg) {

	cout << "***********************" << endl;
	cout << message << " " << arg << endl;
	cout << "***********************" << endl;
}

const char* to_char_array(unsigned long long arg) {
	string tem;
	stringstream out;

	out << arg;
	tem = out.str();
	const char *ans = tem.c_str();
	return ans;
}

char* allocate_str_mem(int len) {
	char *tem;

	if (len <= 0) {
		print("ERROR: Given_memory_length<=0");
		exit (EXIT_FAILURE);
	}
	tem = (char*)malloc(len * sizeof (char));
	if (tem != NULL) {
		memset(tem, 0, len * sizeof (char));
		return tem;
	}
	else {
		print("ERROR: Memory allocation failure");
		exit(EXIT_FAILURE);
	}
}

uint8_t* allocate_uint8_mem(int len) {
	uint8_t *tem;

	if (len <= 0) {
		print("ERROR: Given_memory_length<=0");
		exit(EXIT_FAILURE);
	}
	tem = (uint8_t*)malloc(len * sizeof (uint8_t));
	if (tem != NULL) {
		memset(tem, 0, len * sizeof (uint8_t));
		return tem;
	} 
	else {
		print("ERROR: Memory allocation failure");
		exit (EXIT_FAILURE);
	}
}

struct ip* allocate_ip_mem(int len) {
	struct ip *ip_hdr;

	if (len <= 0) {
		print("ERROR: Given_memory_length<=0");
		exit(EXIT_FAILURE);
	}
	ip_hdr = (ip*)malloc(len * sizeof (uint8_t));
	if (ip_hdr != NULL) {
		memset(ip_hdr, 0, len * sizeof (uint8_t));
		return ip_hdr;
	} 
	else {
		print("ERROR: Memory allocation failure");
		exit (EXIT_FAILURE);
	}
}

struct tcphdr* allocate_tcp_mem(int len) {
	struct tcphdr *tcp_hdr;

	if (len <= 0) {
		print("ERROR: Given_memory_length<=0");
		exit(EXIT_FAILURE);
	}
	tcp_hdr = (tcphdr*)malloc(len * sizeof (uint8_t));
	if (tcp_hdr != NULL) {
		memset(tcp_hdr, 0, len * sizeof (uint8_t));
		return tcp_hdr;
	} 
	else {
		print("ERROR: Memory allocation failure");
		exit (EXIT_FAILURE);
	}
}

void check_server_usage(int argc, char *argv[]) {

	if (argc < 2) {
		cout << "Please try again with the number of threads to be spawn" << endl;
		exit(-1);
	}
	if (atoi(argv[1]) == 0) {
		cout << "Please try again with a valid number of threads to be spawn" << endl;
		exit(-1);
	}
	print_message("Server started");
}

void check_client_usage(int argc, char *argv[]) {

	if (argc < 3) {
		cout << "Please enter the correct number of arguments" << endl;
		cout << "Argument-1: Number of threads to be spawn" << endl;
		cout << "Argument-2: Duration of simulation" << endl;
		exit(-1);
	}
	if (atoi(argv[1]) == 0 || atof(argv[2]) == 0) {
		cout << "Please enter valid arguments" << endl;
		cout << "Argument-1: Number of threads to be spawn" << endl;
		cout << "Argument-2: Duration of simulation" << endl;
		exit(-1);
	}
	print_message("Client started");
}

void time_check(time_t &start_time, double &duration_time, bool &time_exceeded) {
	double elapsed_time;

	if ((elapsed_time = difftime(time(0), start_time)) > duration_time) {
		time_exceeded = true;
	}
}

void handle_exceptions() {

	exit(1);
	while(1) {

	}
}
