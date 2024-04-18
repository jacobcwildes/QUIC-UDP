// Client side implementation of UDP client-server model 
#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <opencv2/opencv.hpp>
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
	uint32_t current_seq = 0;
	uint32_t current_ack = 0;
	struct sockaddr_in servaddr; 
	struct dataframe *parsed_data, *sending;
	cv::Mat image;
	cv::namedWindow("Video Stream");
	cv::resizeWindow("Image", 128, 128);
	
	sending = (dataframe*) malloc(sizeof(dataframe));
	parsed_data = (dataframe*) malloc(sizeof(dataframe));
	
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
	
	std::srand(time(0));
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
	quic.send(sending);
	usleep(1e6);
	
	//Three way handshake
	while(1){
		//1
		sending->seq = (uint32_t) std::rand();
		sending->ack = 0;
		sending->syn = 1;
		sending->fin = 0;
		sending->length = 0;
		quic.send(sending);
		
		//2
		valid = quic.receive_data(parsed_data);
		if (valid == 0){
			std::cout << "Data corrupt, send again" << std::endl;
			continue;
		}
		else if (parsed_data->ack != sending->seq + 1){
			std::cout << "Rands not syncing" << std::endl;
			continue;
		}
		
		//3
		sending->seq = 0;
		sending->ack = 1;
		sending->syn = 0;
		sending->fin = 0;
		sending->length = 0;
		quic.send(sending);
		
		break;
	}
	
	std::cout << std::endl << std::endl << std::endl << "Finished Handshake! Let's begin!" << std::endl;
	usleep(1e6);
	
	
	while(1) {
		
		memset(sending->pixels, 0, MAXDATA*sizeof(char));
		std::cout << "What video do you want to watch?: ";
		std::scanf("%s", sending->pixels);
		
		sending->seq = current_seq;
		sending->ack = 0;
		sending->syn = 0;
		sending->fin = 0;
		sending->length = strlen(sending->pixels);
		quic.send(sending);
		
		
		valid = quic.receive_data(parsed_data);
		if (valid == 0){
			std::cout << "Data corrupt, send again" << std::endl;
		}
		current_seq += sending->length;
		
		
		if (!(strcmp(sending->pixels, "Dyllon"))){
			for (int i = 97; i <= 122; i++) {
				valid = quic.receive_data(parsed_data);
				if (valid == 0){
					std::cout << "Data corrupt, send again" << std::endl;
				}
				
				std::cout << "Movie Data: " << (int) parsed_data->pixels[0] << std::endl;
				
				image = cv::RGBImage(parsed_data->pixels, CV_8U);
				cv::imshow("Dyllon", image);
				cv::waitkey(10);
				
				memset(sending->pixels, 0, MAXDATA*sizeof(char));
				sending->seq = 0;
				sending->ack = parsed_data->seq + parsed_data->length;
				sending->syn = 0;
				sending->fin = 0;
				sending->length = 1;
				sprintf(sending->pixels, "0");
				quic.send(sending);
			}
		}
		if (!(strcmp(sending->pixels, "Finish"))){
			break;
		}
		usleep(5e6);
	}
	
	std::cout << "Finishing up!" << std::endl;
	//Finish
	while(1){
		memset(sending->pixels, 0, MAXDATA*sizeof(char));
		//1
		uint32_t random = (uint32_t) std::rand();
		sending->seq = random;
		sending->ack = random;
		sending->syn = 0;
		sending->fin = 1;
		sending->length = 0;
		quic.send(sending);
		
		//2
		valid = quic.receive_data(parsed_data);
		if (valid == 0){
			std::cout << "Data corrupt, send again" << std::endl;
			//continue;
		}
		else if (!(parsed_data->seq == sending->seq && parsed_data->ack == sending->ack + 1)){
			std::cout << "Rands not syncing" << std::endl;
			continue;
		}
		
		//3
		sending->seq = 0;
		sending->ack = parsed_data->ack;
		sending->syn = 0;
		sending->fin = 0;
		sending->length = 0;
		quic.send(sending);
		
		break;
	}

	close(sockfd); 
	return 0; 
}
