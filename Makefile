CC = g++
CFLAGS = -g -O2 -Wall
LFLAGS =

all:	udp_client udp_server

udp_client:	udp_client.o
	$(CC) $(LFLAGS) -o udp_client udp_client.o

udp_client.o:	udp_client.cpp
	$(CC) $(CFLAGS) -c udp_client.cpp

udp_server:	udp_server.o
	$(CC) $(LFLAGS) -o udp_server udp_server.o

udp_server.o:	udp_server.cpp
	$(CC) $(CFLAGS) -c udp_server.cpp

clean:	
	rm -f *~ *.o udp_server udp_client
