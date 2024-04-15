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
	struct sockaddr_in servaddr, cliaddr; 
	struct dataframe zero, *parsed_data, *sending;
	zero.seq = 0;
	zero.ack = 1;
	zero.syn = 0;
	zero.fin = 0;
	zero.length = 0;
	zero.data = data;
	parsed_data = &zero;
	sending = &zero;

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	}  
	   
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
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
	
	/* -- Now make quic object and begin talking -- */
	
	SimpleQuic quic(MAXHEADER, MAXDATA, MAXDELAY, sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
	
	while (1) {
		std::cout << "Waiting For Data!" << std::endl;
		quic.receive_data(parsed_data);
		
		sprintf(data, "bruh");
		sending->seq = 0;
		sending->ack = 1;
		sending->syn = 0;
		sending->fin = 0;
		sending->length = 0;
		sending->data = parsed_data->data;
		
		std::cout << "Received: " << parsed_data->data << std::endl;
		quic.send(sending);
	}
	

	close(sockfd); 
	return 0; 
}
