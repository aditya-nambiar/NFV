#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>           // close()
#include <string.h>           // strcpy, memset(), and memcpy()

#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_RAW, IPPROTO_IP, IPPROTO_UDP, INET_ADDRSTRLEN
#include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
#include <netinet/udp.h>      // struct udphdr
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <signal.h>
#include <errno.h>            // errno, perror()

using namespace std;


#define BUFFER_SIZE 1024
#define IP4_HDRLEN 20         // IPv4 header length
#define UDP_HDRLEN  8         // UDP header length, excludes data

char *allocate_strmem (int);

void report_error(int arg){
	if(arg<0){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}
}

int main (){
	int sd;
	const int on = 1;
	char *interface;
	struct ifreq ifr;
	int status;	
	sockaddr_in server_sock_addr, client_sock_addr;
	socklen_t g_addr_len;
	int server_port;
	const char *server_address;
	char server_buffer[BUFFER_SIZE];
	int g_reuse;	

	interface = allocate_strmem (40);
	strcpy (interface, "lo");
	if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
		perror ("socket() failed to get socket descriptor for using ioctl() ");
		exit (EXIT_FAILURE);
	}
	memset (&ifr, 0, sizeof (ifr));
	snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
	if (ioctl (sd, SIOCGIFINDEX, &ifr) < 0) {
		perror ("ioctl() failed to find interface ");
		return (EXIT_FAILURE);
	}
	close (sd);
	printf ("Index for interface %s is %i\n", interface, ifr.ifr_ifindex);


	if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
		perror ("socket() failed ");
		exit (EXIT_FAILURE);
	}
	if (setsockopt (sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on)) < 0) {
		perror ("setsockopt() failed to set IP_HDRINCL ");
		exit (EXIT_FAILURE);
	}
	if (setsockopt (sd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof (ifr)) < 0) {
		perror ("setsockopt() failed to bind to interface ");
		exit (EXIT_FAILURE);
	}

	g_reuse = 1;
	g_addr_len = sizeof(sockaddr_in);
	server_port = 5000;
	server_address = "127.0.0.1";
	bzero((char *) &server_sock_addr, sizeof(server_sock_addr));
	server_sock_addr.sin_family = AF_INET;  	
	server_sock_addr.sin_port = htons(server_port); 
	inet_aton(server_address, &server_sock_addr.sin_addr);
	if ((sd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror ("socket() failed to get socket descriptor for using ioctl() ");
		exit (EXIT_FAILURE);
	}	
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));
	signal(SIGPIPE, SIG_IGN);	  
	cout<<ntohs(server_sock_addr.sin_port)<<endl;
	status = bind(sd, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
	report_error(status);	
	while(1){
		cout<<"Server listening"<<endl;
		bzero(server_buffer, BUFFER_SIZE);
		status = recvfrom(sd, server_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
		report_error(status);
		cout<<"Successfully received from "<<htons(client_sock_addr.sin_port)<<endl;
		cout<<status<<endl;
		char tem[5];
		tem[4] = '\0';
		memcpy(tem, server_buffer+8, 4);
		cout<<tem<<endl;
	}

	close (sd);
	return 0;
}

char *allocate_strmem (int len){
	char *tmp;
	if (len <= 0) {
		fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_strmem().\n", len);
		exit (EXIT_FAILURE);
	}
	tmp = (char *) malloc (len * sizeof (char));
	if (tmp != NULL) {
		memset (tmp, 0, len * sizeof (char));
		return (tmp);
	} else {
		fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_strmem().\n");
		exit (EXIT_FAILURE);
	}
}


// #include <iostream>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>           // close()
// #include <string.h>           // strcpy, memset(), and memcpy()

// #include <netdb.h>            // struct addrinfo
// #include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
// #include <sys/socket.h>       // needed for socket()
// #include <netinet/in.h>       // IPPROTO_RAW, IPPROTO_IP, IPPROTO_UDP, INET_ADDRSTRLEN
// #include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
// #include <netinet/udp.h>      // struct udphdr
// #include <arpa/inet.h>        // inet_pton() and inet_ntop()
// #include <sys/ioctl.h>        // macro ioctl is defined
// #include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
// #include <net/if.h>           // struct ifreq

// #include <errno.h>            // errno, perror()

// #include <signal.h>

// using namespace std;

// #define BUFFER_SIZE 1024

// void report_error(int arg){
// 	if(arg<0){
// 		perror("ERROR");
// 		exit(EXIT_FAILURE);
// 	}
// }

// int main ()
// {
// 	int status,  sd;	

// 	sockaddr_in server_sock_addr, client_sock_addr;
// 	socklen_t g_addr_len;

// 	int server_port;
// 	const char *server_address;
// 	char server_buffer[BUFFER_SIZE];

// 	int g_reuse;	
// 	g_reuse = 1;
// 	g_addr_len = sizeof(sockaddr_in);
// 	server_port = 5000;
// 	server_address = "127.0.0.1";
// 	bzero((char *) &server_sock_addr, sizeof(server_sock_addr));
// 	server_sock_addr.sin_family = AF_INET;  	
// 	server_sock_addr.sin_port = htons(server_port); 
// 	inet_aton(server_address, &server_sock_addr.sin_addr);



// 	if ((sd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
// 		perror ("socket() failed to get socket descriptor for using ioctl() ");
// 		exit (EXIT_FAILURE);
// 	}
	
// 	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &g_reuse, sizeof(int));
// 	signal(SIGPIPE, SIG_IGN);	  

// 	cout<<ntohs(server_sock_addr.sin_port)<<endl;
// 	status = bind(sd, (struct sockaddr*)&server_sock_addr, sizeof(server_sock_addr));
// 	report_error(status);	
	
// 	while(1){
// 		cout<<"Server listening"<<endl;
// 		bzero(server_buffer, BUFFER_SIZE);
// 		status = recvfrom(sd, server_buffer, BUFFER_SIZE-1, 0, (sockaddr*)&client_sock_addr, &g_addr_len);
// 		//status = recv(sd, server_buffer, BUFFER_SIZE-1, 0);
// 		report_error(status);
// 		cout<<"Successfully received from "<<htons(client_sock_addr.sin_port)<<endl;
// 		cout<<status<<endl;
// 		char tem[5];
// 		tem[0] = 'a';
// 		tem[1] = 'b';
// 		tem[2] = 'c';
// 		tem[3] = 'd';
// 		tem[4] = 'e';
// 		memcpy(tem, server_buffer+8, 4);
// 		cout<<tem<<endl;
// 		cout<<tem[4]<<endl;
// 	}

// 	close (sd);
// 	return 0;
// }