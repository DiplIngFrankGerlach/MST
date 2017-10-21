# Example Makefile for MST, client/server example
#
# 

VPATH:=../aes

INCLUDES= -I.. -I../aes

CFLAGS= -g -Wall $(INCLUDES)
#For production: CFLAGS= -O2 -Wall
GPP_FLAGS = -g -Wall $(INCLUDES)

OBJECTS=aes.o server.o BufferedSocket.o

CLIENT_OBJECTS = client.o aes.o BufferedSocket.o

server.o: ThreadWorkQueue.h ../Protocol.h BufferedSocket.h ../aes/aes.h BufferedSocket.o

client.o: BufferedSocket.h

Protocol.h: ../DaviesMeyerHash.h ../Util.h ../RandomSource.h



%.o: %.c 
	gcc $(CFLAGS)  -c  $< -o $@ 

%.o: %.cpp
	g++ $(GPP_FLAGS)  -c -o $@ $< 


server:$(OBJECTS)
	g++ $(OBJECTS) -pthread -o server


client: $(CLIENT_OBJECTS)
	g++ $(CLIENT_OBJECTS) -o client 

clean:
	rm $(OBJECTS) $(CLIENT_OBJECTS) server client
