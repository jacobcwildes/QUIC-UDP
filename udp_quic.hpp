#define MAXLINE 10000
#define MAXHEADER 100000
#define MAXDATA 10000
#define MAXDELAY 1000
#define DEFAULT_PORT	8080
#define DEFAULT_HOSTNAME	"localhost"

struct dataframe{
  uint32_t seq;
  uint32_t ack; 
  uint8_t syn; 
  uint8_t fin; 
  uint16_t length; 
  uint16_t checksum;
  char* data;
};


void data_create(char *buffer, struct dataframe input){

	char beforeChecksum[MAXHEADER];
	
	sprintf(beforeChecksum, "%d,%d,%d,%d,%d", input.seq, input.ack, input.syn, input.fin, input.length);
	
	//calc checksum
	uint16_t checksum = input.seq + input.ack + input.syn + input.fin + input.length + (long int)*input.data;
	
	sprintf(buffer, "%s,%d,%s", beforeChecksum, checksum, input.data);
	
}

void send(struct dataframe input, int sockfd, const struct sockaddr *address, int addr_size){
	char frame[MAXHEADER];
	data_create(frame, input);
	std::cout << frame << std::endl;
	sendto(sockfd, (char *)frame, strlen(frame), 
		MSG_CONFIRM, address,  
			addr_size); 
}

dataframe data_parse(char *buf){
	struct dataframe parsed;
	size_t pos = 0;
	std::string buffer = buf;
	std::string delim = ",";
	
	pos = buffer.find(',');
	parsed.seq = (uint32_t) stoi(buffer.substr(0,pos));
	buffer.erase(0, pos + delim.length());
	
	pos = buffer.find(',');
	parsed.ack = (uint32_t) stoi(buffer.substr(0,pos));
	buffer.erase(0, pos + delim.length());
	
	pos = buffer.find(',');
	parsed.syn = (uint8_t) stoi(buffer.substr(0,pos));
	buffer.erase(0, pos + delim.length());
	
	pos = buffer.find(',');
	parsed.fin = (uint8_t) stoi(buffer.substr(0,pos));
	buffer.erase(0, pos + delim.length());
	
	pos = buffer.find(',');
	parsed.length = (uint16_t) stoi(buffer.substr(0,pos));
	buffer.erase(0, pos + delim.length());
	
	pos = buffer.find(',');
	parsed.checksum = (uint16_t) stoi(buffer.substr(0,pos));
	buffer.erase(0, pos + delim.length());
	
	pos = buffer.find(',');
	parsed.data = (char *) buffer.substr(0,pos).c_str();
	buffer.erase(0, pos + delim.length());
	
	return parsed;
}

int checksum_valid(struct dataframe input){
	if ((input.seq + input.ack + input.syn + input.fin + input.length + (long int)*input.data) == input.checksum) return 1;
	return 0;
}

dataframe receive_data(int sockfd, struct sockaddr *address){
	char recv[MAXLINE];
	struct dataframe parsed_data;
	socklen_t len;
	int valid;
	uint64_t n = recvfrom(sockfd, (char *)recv, MAXHEADER+MAXDATA,  
			MSG_WAITALL, address, 
			&len); 
	recv[n] = '\0'; 
	std::cout << recv << std::endl;
	parsed_data = data_parse(recv);
	valid = checksum_valid(parsed_data);
	std::cout << "Valid Checksum: " << valid << std::endl;
	
	return parsed_data;
}
