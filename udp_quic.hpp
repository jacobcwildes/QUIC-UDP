struct dataframe{
  uint32_t seq;
  uint32_t ack; 
  uint8_t syn; 
  uint8_t fin; 
  uint16_t length; 
  uint16_t checksum;
  char* data;
};

class SimpleQuic {
	public:
		int max_header;
		int max_data;
		int max_delay;
		int socketfd;
		struct sockaddr *address;
		int addr_size;
		
		SimpleQuic(int max_header, int max_data, int max_delay, int socketfd, struct sockaddr *address, int addr_size) {
			this->max_header = max_header;
			this->max_data = max_data;
			this->max_delay = max_delay;
			this->socketfd = socketfd;
			this->address = address;
			this->addr_size = addr_size;
		}
		
		void data_create(char *buffer, struct dataframe input){

			char beforeChecksum[max_header];
			
			sprintf(beforeChecksum, "%d,%d,%d,%d,%d", input.seq, input.ack, input.syn, input.fin, input.length);
			
			//calc checksum
			uint16_t checksum = input.seq + input.ack + input.syn + input.fin + input.length + (long int)*input.data;
			
			sprintf(buffer, "%s,%d,%s", beforeChecksum, checksum, input.data);
			
		}

		void send(struct dataframe input){
			char frame[max_header];
			data_create(frame, input);
			std::cout << frame << std::endl;
			sendto(socketfd, (char *)frame, strlen(frame), 
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

		dataframe receive_data(){
			char recv[max_data + max_header];
			struct dataframe parsed_data;
			socklen_t len;
			int valid;
			uint64_t n = recvfrom(socketfd, (char *)recv, max_header+max_data,  
					MSG_WAITALL, address, 
					&len); 
			recv[n] = '\0'; 
			std::cout << recv << std::endl;
			parsed_data = data_parse(recv);
			valid = checksum_valid(parsed_data);
			std::cout << "Valid Checksum: " << valid << std::endl;
			
			return parsed_data;
		}
};


