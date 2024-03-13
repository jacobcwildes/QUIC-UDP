// Client side implementation of UDP client-server model 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
   
#define PORT     8080 
#define MAXLINE 1024 
#define MAXDATA 920
#define MAXDELAY 1000

struct dataframe{
  uint32_t seq;
  uint32_t ack; 
  uint8_t flags; 
  uint16_t length; 
  uint16_t checksum;
  char* data;
};

void data_create(char *buffer, uint32_t seq, uint32_t ack, uint8_t syn, uint8_t fin, uint16_t length, char* data){
	memset(buffer, 0, MAXLINE);
	memcpy(buffer, &seq, 4);
	memcpy(buffer+4, &ack, 4);
	uint8_t flags = 0;
	if (syn) flags |= 2;
	if (fin) flags |= 1;
	memcpy(buffer+8, &flags, 1);
	memcpy(buffer+9, &length, 2);
	
	//calc checksum
	uint16_t *checksum = 0;
	for (int i = 0; i < MAXLINE; i++) checksum += buffer[i];
	
	memcpy(buffer+11, checksum, 2);
	memcpy(buffer+13, data, MAXDATA);
}

dataframe data_parse(char *buffer){
	struct dataframe parsed;
	memcpy(&parsed.seq, buffer, 4);
	memcpy(&parsed.ack, buffer+4, 4);
	memcpy(&parsed.flags, buffer+8, 1);
	memcpy(&parsed.length, buffer+9, 2);
	memcpy(&parsed.checksum, buffer+11, 2);
	memcpy(&parsed.data, buffer+13, MAXDATA);
	return parsed;
}

uint64_t send_recv(char *sendbuffer, char* recvbuffer, int sockfd, struct sockaddr_in servaddr){
	socklen_t len; 
    sendto(sockfd, (char *)sendbuffer, strlen(sendbuffer), 
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
			sizeof(servaddr)); 
	   
	uint64_t n = recvfrom(sockfd, (char *)recvbuffer, MAXLINE,  
		    MSG_WAITALL, (struct sockaddr *) &servaddr, 
		    &len); 
	recvbuffer[n] = '\0'; 
	return n;
}


// Driver code 
int main() { 
    int sockfd; 
    char recv[MAXLINE]; 
    char data[MAXLINE];
    struct dataframe parsed_data;
    struct sockaddr_in     servaddr; 
   
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
   
    memset(&servaddr, 0, sizeof(servaddr)); 
       
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = inet_addr("192.168.57.128"); 
       
    int n;
    socklen_t len; 
    
    //Three way handshake
    uint32_t rand = 654651;
    data_create(data, rand, 0, 1, 0, 0, NULL);
    send_recv(data, recv, sockfd, servaddr);
    parsed_data = data_parse((char *)&recv);
    if (parsed_data.ack != rand + 1 || (parsed_data.flags & 0b11111101) != 1) {
    	std::cout << "Handshake Failed! \n";
    	return -1;
    }
    
    data_create(data, 0, 1, 0, 0, 0, NULL);
    sendto(sockfd, (char *)data, strlen(data), 
		MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
			sizeof(servaddr)); 
			
			
    while (1) {
    
    
		
    }
    
    close(sockfd); 
    return 0; 
}
