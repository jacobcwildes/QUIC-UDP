// Server side implementation of UDP client-server model 
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

//Concatenate the header and data together
char *conc_head(char *buffer, uint32_t seq, uint32_t ack, uint16_t length, bool syn, bool fin);
   
//Struct to hold client information
struct client_head{
    uint32_t seq;
    uint32_t ack;
    uint16_t length;
    uint8_t flags;
};

//Parse data from client
//void data_parse(char *buffer, uint32_t seq, uint32_t ack, uint16_t length, uint8_t flags, cient_head client);

// Driver code 
int main() { 
    int sockfd; 
    char buffer[MAXLINE]; 
    char *data_out;
    const char *hello = "Hello from server"; 
    struct sockaddr_in servaddr, cliaddr; 
       
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
       
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
       
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    socklen_t len;
  int n; 
   
    len = sizeof(cliaddr);  //len is value/result 
    
    //Handshake for inner TCP
    
    //loop until done
    while(1)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                &len); 
        buffer[n] = '\0'; 
        
        printf("Client : %s\n", buffer); 
        data_out = conc_head(buffer, 19, 18, 74, false, true); //change this to be dynamic
        sendto(sockfd, (const char *)hello, strlen(hello),  
            MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                len); 
        std::cout<<"Hello message sent."<<std::endl;  
    }
       
    return 0; 
}

char *conc_head(char *buffer, uint32_t seq, uint32_t ack, uint16_t length, bool syn, bool fin)
{
    uint8_t flags = 0;
    char *data_out;
    uint16_t check;
    //clear memory for output data
    memset(&data_out, 0, sizeof(*buffer));
    
    //assign values to bools
    if(syn) flags |= 2;
    
    if(fin) flags |= 1;
    
    
    //Make checksum by adding ack, seq, flags, and length together
    check = seq + ack + length + flags;
    
    //concatenate into header
    memcpy(data_out, (void *)seq, 4);
    memcpy(data_out+32, (void *)ack, 4);
    memcpy(data_out+64, (void *)flags, 1);
    memcpy(data_out+72, (void *)length, 2);
    memcpy(data_out+88, (void *)check, 2);
    
    //Verify header
    std::cout << &data_out << std::endl;
    
    return data_out;
}
/*
void data_parse(char *buffer, uint32_t seq, uint32_t ack, uint16_t length, uint8_t flags, cient_head client)
{
    
}
*/
