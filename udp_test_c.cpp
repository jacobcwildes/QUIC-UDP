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
   
#define MAXLINE 1000
#define MAXHEADER 200
#define MAXDATA 50000
#define MAXDELAY 1000
#define DEFAULT_PORT	8080
#define DEFAULT_HOSTNAME	"localhost"


// Driver code 
int main() { 
	int sockfd; 
	int valid;
	struct sockaddr_in servaddr; 
	struct dataframe zero, *parsed_data, *sending;
	
	zero.seq = 0;
	zero.ack = 1;
	zero.syn = 0;
	zero.fin = 0;
	zero.length = 0;
	parsed_data = &zero;
	sending = &zero;
	
	sending->seq = 0;
	sending->ack = 0;
	sending->syn = 0;
	sending->fin = 0;
	sending->length = 0;
	sending->checksum = 0;
	sending->pixels = (char*) calloc(MAXDATA,sizeof(char));
	
	parsed_data->seq = 0;
	parsed_data->ack = 0;
	parsed_data->syn = 0;
	parsed_data->fin = 0;
	parsed_data->length = 0;
	parsed_data->checksum = 0;
	parsed_data->pixels = (char*) calloc(MAXDATA,sizeof(char));
	
	   
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
	
	SimpleQuic quic(MAXHEADER, MAXDATA, MAXDELAY, sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
	//Wake up call
	sending->seq = 0;
	sending->ack = 0;
	sending->syn = 0;
	sending->fin = 0;
	sending->length = 0;
	quic.send(sending);
	usleep(1e6);
	
	
	
	
	
	while(1) {
		std::cout << "------------------" << std::endl;
		sending->seq = 0;
		sending->ack = 1;
		sending->syn = 0;
		sending->fin = 0;
		sending->length = MAXDATA;
		for (int i = 0; i < MAXDATA; i++) sending->pixels[i] = 5;
		quic.send(sending);
		valid = quic.receive_data(parsed_data);
		if (valid == 0){
			std::cout << "Data corrupt, send again" << std::endl;
		}
		//usleep(5e6);
	}
	

	close(sockfd); 
	return 0; 
}
