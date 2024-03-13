#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFFER_SIZE	256

/* Default port to listen on */
#define DEFAULT_PORT	31337

int main(int argc, char **argv) {

	int socket_fd;
	struct sockaddr_in server_addr, client_addr;
	int port=DEFAULT_PORT;
	int n;
	socklen_t client_len;
	char buffer[BUFFER_SIZE];
	int optval;
	struct hostent *hostp;
	char *hostaddrp; /* IP address */
	unsigned short srcport; //Source port
	int count = 0; //Only send back necessary stuff, not blanks

	printf("Starting UDP server on port %d\n",port);

	/* Open a socket to listen on */
	/* AF_INET means an IPv4 connection */
	/* SOCK_DGRAM is UDP */
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd<0) {
		fprintf(stderr,"Error opening socket! %s\n",
			strerror(errno));
	}

	/* setsockopt: avoid TIME WAIT?? */
	optval = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
		(const void *)&optval , sizeof(int));

	/* Set up the server address to listen on */
	memset(&server_addr,0,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	/* Convert the port we want to network byte order */
	server_addr.sin_port=htons(port);

	/* Bind to the port */
	if (bind(socket_fd, (struct sockaddr *) &server_addr,
		sizeof(server_addr)) <0) {
		fprintf(stderr,"Error binding! %s\n", strerror(errno));
	}

	client_len=sizeof(client_addr);

	while(1) {

		/* Wait until we receive a UDP packet */
		memset(buffer,0,BUFFER_SIZE);


		/* Read from socket */
        //use the recvfrom call instead simply to 
        n = recvfrom(socket_fd, buffer, (BUFFER_SIZE - 1), 0, (struct sockaddr *) &client_addr, &client_len);

		if (n==0) {
			fprintf(stderr,"Connection to client lost\n\n");
			break;
		}
		else if (n<0) {
			fprintf(stderr,"Error reading from socket %s\n",
				strerror(errno));
			break;
		}
		
		//Get the host name, address, and source port
		hostp = gethostbyaddr((const char *)&client_addr.sin_addr.s_addr, sizeof(client_addr.sin_addr.s_addr), AF_INET);
		
		hostaddrp = inet_ntoa(client_addr.sin_addr);
		
		//Need to convert from network order back to host
		srcport = ntohs(client_addr.sin_port);
		
		//Print the fetched information
		printf("Host name: %s Address: %s Source Port: %d\n\r", hostp->h_name, hostaddrp, srcport);

		/* Print the message we received */
		printf("Message from client: %s\n",buffer);
		
		//Uppercase the received message and send it back
		for(int i = 0; i < BUFFER_SIZE - 1; i++)
		{
	        if(buffer[i] != '\0')
	        {
                buffer[i] = toupper(buffer[i]);
                count++;
            }
            else break;
        }
        //printf("Count: %d\n\r", count);
        
        //This most likely is not necessary, but now only send back the message length, not an entire buffer
        char new_buffer[count];
        
        for(int i = 0; i < count + 1; i++)
        {
            new_buffer[i] = buffer[i];
        }  
        
        //Send the uppercase message back to the client
        n = sendto(socket_fd, new_buffer, strlen(new_buffer), 0, (struct sockaddr *)&client_addr, client_len);
        
		if (n==0) {
		    fprintf(stderr,"Connection to client lost\n\n");
			break;
		}
		else if (n<0) {
			fprintf(stderr,"Error writing to socket %s\n",
				strerror(errno));
			break;
		}
        
        //If we receive "BYE" from client, close the server, also 
        //make sure that we account for the newline
        if(!strncmp(buffer, "BYE\n", 4))
        {
            close(socket_fd);
            return 0;
        }
        
        count = 0; //Reset count to 0 for next message

	}

	printf("Exiting server\n\n");

	/* Close the sockets */
	close(socket_fd);

	return 0;
}
