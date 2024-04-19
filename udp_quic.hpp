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
		
		int data_create(struct dataframe *input){ //tested and is working

			
			//calc checksum
			uint16_t data_sum = 0;
			for (int i = 0; i < input->length; i++) data_sum += input->pixels[i];
			uint16_t checksum = input->seq + input->ack + input->syn + input->fin + input->length + data_sum;
			
			char beforeChecksum[max_header];
			
			sprintf(beforeChecksum, "%d,%d,%d,%d,%d,%d,", input->seq, input->ack, input->syn, input->fin, input->length,checksum);
			int len = strlen(beforeChecksum);
			
			std::cout << "Send: " << beforeChecksum;
			if (input->length <=100) std::cout << input->pixels;
			std::cout << std::endl;
			
			sprintf(data, "%s%s", beforeChecksum, input->pixels);
			return len;
			
		}
		
		void send(struct dataframe *input){
			std::cout << "--------------------" << std::endl;
			int head_length = data_create(input);
			//std::cout << "head length: " << head_length << std::endl;
			sendto(socketfd, data, strlen(data), 
						MSG_CONFIRM, address,  
						addr_size); 
						
		}

		void data_parse(struct dataframe *parsed){ //tested and working
			size_t pos = 0;
			std::string buffer = data;
			std::string delim = ",";
			//std::cout << buffer << std::endl;
			pos = buffer.find(',');
			parsed->seq = (uint32_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			std::cout << "Recv: " << parsed->seq << ",";
			
			pos = buffer.find(',');
			parsed->ack = (uint32_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			std::cout << parsed->ack << ",";
			
			pos = buffer.find(',');
			parsed->syn = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			std::cout << parsed->syn << ",";
			
			pos = buffer.find(',');
			parsed->fin = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			std::cout << parsed->fin << ",";
			
			pos = buffer.find(',');
			parsed->length = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			std::cout << parsed->length << ",";
			
			pos = buffer.find(',');
			parsed->checksum = (uint16_t) stoi(buffer.substr(0,pos));
			buffer.erase(0, pos + delim.length());
			std::cout << parsed->checksum;
			
			
			//pos = buffer.find(',');
			strcpy(parsed->pixels,buffer.substr(0,parsed->length).c_str());
			if (parsed->length <= 100) std::cout << "," << parsed->pixels;
			std::cout << std::endl;
			
			std::cout << "Parsed Buffer pixel Count: " << strlen(parsed->pixels) << std::endl;
			
			
			
		}

		int checksum_valid(struct dataframe *input){ //this is tested and working
			uint16_t data_sum = 0; 
			for (int i = 0; i < input->length; i++) data_sum += input->pixels[i];
			uint16_t checksum = input->seq + input->ack + input->syn + input->fin + input->length + data_sum;
			std::cout << "Calc Checksum: " << checksum << std::endl;
			std::cout << "Pixel Count: " << strlen(input->pixels) << std::endl;
			if (checksum == input->checksum) return 1;
			return 0;
		}

		int receive_data(struct dataframe *parsed_data){
			int valid;
			memset(data, 0, (max_data+max_header)*sizeof(char));
			n = recvfrom(socketfd, data, max_header+max_data,  
					MSG_WAITALL, address, 
					&len);	
			//data[n] = '\0';

			std::cout << "Datalength: " << strlen(data) << std::endl;
			data_parse(parsed_data);
			valid = checksum_valid(parsed_data);
			if (valid) std::cout << "Valid Checksum :)" << std::endl;
			else std::cout << "Invalid Checksum :(" << std::endl;
			return valid;
		}
		
		
};

