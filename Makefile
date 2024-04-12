CC = g++
CFLAGS = -g -O2 -Wall
LFLAGS =

all:	udp_client udp_server

test:	udp_test_c udp_test_s

udp_client:	udp_client.o
	$(CC) $(LFLAGS) -o udp_client udp_client.o

udp_client.o:	udp_client.cpp
	$(CC) $(CFLAGS) -c udp_client.cpp

udp_server:	udp_server.o
	$(CC) $(LFLAGS) -o udp_server udp_server.o

udp_server.o:	udp_server.cpp
	$(CC) $(CFLAGS) -c udp_server.cpp
	
udp_test_c:	udp_test_c.o
	$(CC) $(LFLAGS) -o udp_test_c udp_test_c.o

udp_test_c.o:	udp_test_c.cpp
	$(CC) $(CFLAGS) -c udp_test_c.cpp
	
udp_test_s:	udp_test_s.o
	$(CC) $(LFLAGS) -o udp_test_s udp_test_s.o

udp_test.o:	udp_test_s.cpp
	$(CC) $(CFLAGS) -c udp_test_s.cpp

clean:	
	rm -f *~ *.o udp_server udp_client udp_test_c udp_test_s
	
	
	
	
