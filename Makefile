CC = g++
CFLAGS = `pkg-config --cflags opencv4`
LFLAGS = `pkg-config --libs opencv4`

all:	udp_client udp_server

test:	udp_test_c udp_test_s

udp_client:	udp_client.o
	$(CC) -o udp_client udp_client.o  $(LFLAGS)

udp_client.o:	udp_client.cpp
	$(CC) $(CFLAGS) -c udp_client.cpp

udp_server:	udp_server.o
	$(CC) -o udp_server udp_server.o  $(LFLAGS)

udp_server.o:	udp_server.cpp
	$(CC) $(CFLAGS) -c udp_server.cpp
	
udp_test_c:	udp_test_c.o
	$(CC) -o udp_test_c udp_test_c.o  $(LFLAGS)

udp_test_c.o:	udp_test_c.cpp
	$(CC) $(CFLAGS) -c udp_test_c.cpp 
	
udp_test_s:	udp_test_s.o
	$(CC) -o udp_test_s udp_test_s.o  $(LFLAGS)

udp_test_s.o:	udp_test_s.cpp
	$(CC) $(CFLAGS) -c udp_test_s.cpp

clean:	
	rm -f *~ *.o udp_server udp_client udp_test_c udp_test_s
	
	
	
	
