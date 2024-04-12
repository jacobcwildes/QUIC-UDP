// Client side implementation of UDP client-server model 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include "udp_quic.hpp"
   
#define MAXLINE 10000
#define MAXHEADER 100000
#define MAXDATA 10000
#define MAXDELAY 1000
#define DEFAULT_PORT	8080
#define DEFAULT_HOSTNAME	"localhost"

// Driver code 
int main() { 
	int sockfd; 
	
	char data[MAXDATA];
	struct sockaddr_in servaddr; 
	   
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	   
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(DEFAULT_PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY;
	
	//THIS IS HOW WE WILL SEND
	//first assemble frame struct
	sprintf(data, "bruh");
	struct dataframe sending;
	sending.seq = 0;
	sending.ack = 1;
	sending.syn = 0;
	sending.fin = 0;
	sending.length = 0;
	sending.data = data;
	
	send(sending, sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr));

	close(sockfd); 
	return 0; 
}
