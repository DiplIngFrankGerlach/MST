# Example Makefile for MST, client/server example
#
# 

VPATH:=../aes

INCLUDES= -I.. -I../aes

CFLAGS= -g -Wall $(INCLUDES)
#For production: CFLAGS= -O2 -Wall
GPP_FLAGS = -g -Wall $(INCLUDES) -std=c++11

OBJECTS=aes.o server.o BufferedSocket.o

CLIENT_OBJECTS = client.o aes.o BufferedSocket.o

OBJECTS_SERVER_FULL = aes.o serverFull.o BufferedSocket.o MST_Socket.o KeyReader.o
OBJECTS_CLIENT_FULL = aes.o clientFull.o BufferedSocket.o MST_Socket.o KeyReader.o

OBJECTS_KR_TEST = KeyReader.o krTest.o


server.o: ThreadWorkQueue.h ../Protocol.h BufferedSocket.h ../aes/aes.h BufferedSocket.o

client.o: BufferedSocket.h

Protocol.h: ../DaviesMeyerHash.h ../Util.h ../RandomSource.h



%.o: %.c 
	gcc $(CFLAGS)  -c  $< -o $@ 

%.o: %.cpp
	g++ $(GPP_FLAGS)  -c -o $@ $< 


server:$(OBJECTS)
	g++ $(OBJECTS) -pthread -o server

serverFull:$(OBJECTS_SERVER_FULL)
	g++ $(OBJECTS_SERVER_FULL) -pthread -o serverFull


client: $(CLIENT_OBJECTS)
	g++ $(CLIENT_OBJECTS) -o client 

clientFull: $(OBJECTS_CLIENT_FULL)
	g++ -g $(OBJECTS_CLIENT_FULL) -o clientFull 

krTest: $(OBJECTS_KR_TEST)
	g++ -g $(OBJECTS_KR_TEST) -o krTest 

clean:
	rm $(OBJECTS) $(CLIENT_OBJECTS) $(OBJECTS_SERVER_FULL) $(OBJECTS_CLIENT_FULL) server client serverFull clientFull
