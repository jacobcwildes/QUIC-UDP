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
		int n;
		socklen_t len;
		
		
		SimpleQuic(int max_header, int max_data, int max_delay, int socketfd, struct sockaddr *address, int addr_size) {
			this->max_header = max_header;
			this->max_data = max_data;
			this->max_delay = max_delay;
			this->socketfd = socketfd;
			this->address = address;
			this->addr_size = addr_size;
		}
		
		void data_create(char *buffer, struct dataframe *input){

			char beforeChecksum[max_header];
			
			sprintf(beforeChecksum, "%d,%d,%d,%d,%d", input->seq, input->ack, input->syn, input->fin, input->length);
			
			//calc checksum
			int data_sum = 0;
			for (long int i = 0; i < (long int) strlen(input->data); i++) data_sum += (int) input->data[i];
			uint16_t checksum = input->seq + input->ack + input->syn + input->fin + input->length + data_sum;
			
			sprintf(buffer, "%s,%d,%s", beforeChecksum, checksum, input->data);
			
		}
		
		void send(struct dataframe *input){
			char frame[max_header];
			data_create(frame, input);
			std::cout << frame << std::endl;
			sendto(socketfd, (char *)frame, strlen(frame), 
						MSG_CONFIRM, address,  
						addr_size); 
		}

		void data_parse(char *buf, struct dataframe *parsed){
			size_t pos = 0;
			std::string buffer = buf;
			std::string delim = ",";
			
			pos = buffer.find(',');
			parsed->seq = (uint32_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->ack = (uint32_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->syn = (uint8_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->fin = (uint8_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->length = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->checksum = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->data = (char *) buffer.substr(0,pos).c_str();
			buffer.erase(0, pos + delim.length());
		}

		int checksum_valid(struct dataframe *input){
			std::cout << input->data << std::endl;
			int data_sum = 0; 
			for (long int i = 0; i < (long int) strlen(input->data); i++) data_sum += (int) input->data[i];
			uint16_t checksum = input->seq + input->ack + input->syn + input->fin + input->length + data_sum;
			if (checksum == input->checksum) return 1;
			return 0;
		}

		void receive_data(struct dataframe *parsed_data){
			char recv[max_data + max_header];
			int valid;
			n = recvfrom(socketfd, (char *)recv, max_header+max_data,  
					MSG_WAITALL, address, 
					&len);	
			recv[n] = '\0'; 
			std::cout << recv << std::endl;
			data_parse(recv, parsed_data);
			valid = checksum_valid(parsed_data);
			std::cout << "Valid?: " << valid << std::endl;
		}
};


