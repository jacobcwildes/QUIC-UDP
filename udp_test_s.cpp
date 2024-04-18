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
   
#define MAXLINE 10000
#define MAXHEADER 200
#define MAXDATA 50000
#define MAXDELAY 1000
#define DEFAULT_PORT	8080
#define DEFAULT_HOSTNAME	"localhost"

// Driver code 
int main() { 
	int sockfd; 
	char data[MAXDATA];
	int valid;
	uint32_t current_ack = 0;
	uint32_t current_seq = 0;
	cv::Mat image;
	struct sockaddr_in servaddr, cliaddr; 
	struct dataframe *parsed_data, *sending;
	
	
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
	
	//wake up
	quic.receive_data(parsed_data);
	
	//Three way handshake
	while(1){
		
		//1
		valid = quic.receive_data(parsed_data);
		if (valid == 0){
			std::cout << "Data corrupt, send again" << std::endl;
			continue;
		}
		
		//2
		sending->seq = (uint32_t) std::rand();
		sending->ack = parsed_data->seq + 1;
		sending->syn = 1;
		sending->fin = 0;
		sending->length = 0;
		quic.send(sending);
		
		//3
		valid = quic.receive_data(parsed_data);
		if (valid == 0){
			std::cout << "Data corrupt, send again" << std::endl;
			continue;
		}
		else if (!(parsed_data->syn == 0 && parsed_data->seq == 0 && parsed_data->ack == 1)){
			std::cout << "Got bad ending shake" << std::endl;
			continue;
		}
		
		break;
	}
	
	std::cout << std::endl << std::endl << std::endl << "Finished Handshake! Let's begin!" << std::endl;
	
	while (1) {
		
		valid = quic.receive_data(parsed_data);
		if (valid == 0){
			std::cout << "Data corrupt, send again" << std::endl;
		}
		memset(sending->pixels, 0, MAXDATA*sizeof(char));
		sending->seq = 0;
		sending->ack = parsed_data->seq + parsed_data->length;
		sending->syn = 0;
		sending->fin = 0;
		sending->length = 1;
		sprintf(sending->pixels, "0");
		quic.send(sending);
		
		if (!(strcmp(parsed_data->pixels, "Dyllon"))){
			
			cv::VideoCapture cap("Dyllon.mp4");
			if ( !cap.isOpened() )  // isOpened() returns true if capturing has been initialized.
			{
				std::cout << "Cannot open the video file. \n";
				return -1;
			}
			
			for (int i = 0; i < 256; i++){
				if (!cap.read(image)) // if not success, break loop
				// read() decodes and captures the next frame.
				{
					cout<<"\n Cannot read the video file. \n";
					break;
				}
				cv::resize(image, image, (128,128));
				//Do rest later heheh
				
			}
		
		
			//Do Dyllon Video
			for (int i = 97; i <= 122; i++){
				memset(sending->pixels, (i-97)*(250/(122-97)), 128*128*3*sizeof(char));
				//sending->pixels[0] = i;
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
			}
		}
		if (!(strcmp(parsed_data->pixels, "Finish"))){
			break;
		}
	}
	
	std::cout << "Finishing up!" << std::endl;
	//Finish
	while(1){
		//1
		valid = quic.receive_data(parsed_data);
		if (valid == 0){
			std::cout << "Data corrupt, send again" << std::endl;
			//continue;
		}
		
		//2
		sending->seq = parsed_data->seq;
		sending->ack = parsed_data->ack + 1;
		sending->syn = 1;
		sending->fin = 0;
		sending->length = 0;
		quic.send(sending);
		
		//3
		valid = quic.receive_data(parsed_data);
		if (valid == 0){
			std::cout << "Data corrupt, send again" << std::endl;
			//continue;
		}
		else if (!(parsed_data->fin == 0 && parsed_data->seq == 0 && parsed_data->ack == sending->ack)){
			std::cout << "Got bad ending shake" << std::endl;
			continue;
		}
		
		break;
	}

	close(sockfd); 
	return 0; 
}
