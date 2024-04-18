struct dataframe{
  uint32_t seq;
  uint32_t ack; 
  uint16_t syn; 
  uint16_t fin; 
  uint16_t length; 
  uint16_t checksum;
  char *pixels;
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
		char *data;
		
		
		SimpleQuic(int max_header, int max_data, int max_delay, int socketfd, struct sockaddr *address, int addr_size) {
			this->max_header = max_header;
			this->max_data = max_data;
			this->max_delay = max_delay;
			this->socketfd = socketfd;
			this->address = address;
			this->addr_size = addr_size;
			data = (char*) calloc((max_data+max_header),sizeof(char));
		}
		
		void data_create(struct dataframe *input){ //tested and is working

			char beforeChecksum[max_header];
			
			sprintf(beforeChecksum, "%d,%d,%d,%d,%d", input->seq, input->ack, input->syn, input->fin, input->length);
			
			//calc checksum
			uint16_t data_sum = 0;
			for (int i = 0; i < max_data; i++) data_sum += input->pixels[i];
			uint16_t checksum = input->seq + input->ack + input->syn + input->fin + input->length + data_sum;
			
			sprintf(data, "%s,%d,%s", beforeChecksum, checksum, input->pixels);
			std::cout << "Data Printout: " << input->pixels << std::endl;
			
		}
		
		void send(struct dataframe *input){
			data_create(input);
			std::cout << "Sending: " << data << std::endl;
			sendto(socketfd, data, strlen(data), 
						MSG_CONFIRM, address,  
						addr_size); 
			std::cout << "Sent!" << std::endl;
						
		}

		void data_parse(struct dataframe *parsed){ //tested and working
			size_t pos = 0;
			std::string buffer = data;
			std::string delim = ",";
			
			pos = buffer.find(',');
			parsed->seq = (uint32_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->ack = (uint32_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->syn = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->fin = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->length = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			pos = buffer.find(',');
			parsed->checksum = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			
			
			pos = buffer.find(',');
			strcpy(parsed->pixels,buffer.substr(0,pos).c_str());
			buffer.erase(0, pos + delim.length());
		}

		int checksum_valid(struct dataframe *input){ //this is tested and working
			uint16_t data_sum = 0; 
			for (int i = 0; i < max_data; i++) data_sum += input->pixels[i];
			uint16_t checksum = input->seq + input->ack + input->syn + input->fin + input->length + data_sum;

			if (checksum == input->checksum) return 1;
			return 0;
		}

		int receive_data(struct dataframe *parsed_data){
			int valid;
			std::cout << "Receiving! " << std::endl;
			n = recvfrom(socketfd, data, max_header+max_data,  
					MSG_WAITALL, address, 
					&len);	
			data[n] = '\0';

			std::cout << "Received: " << data << std::endl;
			data_parse(parsed_data);
			valid = checksum_valid(parsed_data);
			if (valid) std::cout << "Valid Checksum :)" << std::endl;
			else std::cout << "Invalid Checksum :(" << std::endl;
			return valid;
		}
		
};


