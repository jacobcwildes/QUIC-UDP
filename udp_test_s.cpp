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

	struct dataframe parsed_data;
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
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
		sizeof(servaddr)) < 0 ) 
	{ 
	    perror("bind failed"); 
	    exit(EXIT_FAILURE); 
	} 
	
	SimpleQuic quic(MAXHEADER, MAXDATA, MAXDELAY, sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
	
	parsed_data = quic.receive_data();

	close(sockfd); 
	return 0; 
}
