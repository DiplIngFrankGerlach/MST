# Example Makefile for MST, client/server example
#
# 

VPATH:=../aes:..

INCLUDES= -I.. -I../aes

#for debugging:
CFLAGS= -g -Wall $(INCLUDES)
GPP_FLAGS = -g -Wall $(INCLUDES) -std=c++11


#for production:
#GPP_FLAGS = -O2 -Wall $(INCLUDES) -std=c++11
#CFLAGS= -O2 -Wall


OBJECTS=../aes/aes.o server.o BufferedSocket.o ../GeneralUtil.o

CLIENT_OBJECTS = client.o ../aes/aes.o BufferedSocket.o ../GeneralUtil.o 

OBJECTS_SERVER_FULL = ../aes/aes.o serverFull.o BufferedSocket.o MST_Socket.o KeyReader.o ../GeneralUtil.o
OBJECTS_CLIENT_FULL = ../aes/aes.o clientFull.o BufferedSocket.o MST_Socket.o KeyReader.o ../GeneralUtil.o

OBJECTS_KR_TEST = KeyReader.o krTest.o


server.o: ThreadWorkQueue.h ../Protocol.h BufferedSocket.h ../aes/aes.h BufferedSocket.o ../GeneralUtil.o

client.o: BufferedSocket.h

Protocol.h: ../DaviesMeyerHash.h ../Util.h ../RandomSource.h

LIBS=-lsocket -lresolv -lnsl

%.o: %.c 
	gcc $(CFLAGS)  -c  $< -o $@ 

%.o: %.cpp
	g++ $(GPP_FLAGS)  -c -o $@ $< 


server:$(OBJECTS)
	g++ $(OBJECTS) -pthread -o server $(LIBS)

serverFull:$(OBJECTS_SERVER_FULL)
	g++ $(OBJECTS_SERVER_FULL) -pthread -o serverFull $(LIBS)


client: $(CLIENT_OBJECTS)
	g++ $(CLIENT_OBJECTS) -o client $(LIBS)

clientFull: $(OBJECTS_CLIENT_FULL)
	g++ -g $(OBJECTS_CLIENT_FULL) -o clientFull $(LIBS)

krTest: $(OBJECTS_KR_TEST)
	g++ -g $(OBJECTS_KR_TEST) -o krTest 

clean:
	rm $(OBJECTS) $(CLIENT_OBJECTS) $(OBJECTS_SERVER_FULL) $(OBJECTS_CLIENT_FULL) server client serverFull clientFull
