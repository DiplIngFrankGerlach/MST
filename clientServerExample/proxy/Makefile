
#
# 

VPATH:=../../aes ../ ../../

INCLUDES= -I../.. -I.. -I../../aes

CFLAGS= -g -Wall $(INCLUDES)
#For production: CFLAGS= -O2 -Wall
GPP_FLAGS = -g -Wall $(INCLUDES) -std=c++11


OBJECTS_SERVER = aes.o TCPServerTest.o BufferedSocket.o MST_Socket.o \
                 KeyReader.o TCPServer.o

OBJECTS_PROXY_CLIENT = aes.o proxyClient.o BufferedSocket.o MST_Socket.o \
                       TCPServer.o KeyReader.o TCPUtil.o

OBJECTS_PROXY_SERVER = aes.o proxyServer.o BufferedSocket.o MST_Socket.o \
                       TCPServer.o KeyReader.o TCPUtil.o

OBJS_TCP_CLIENT = tcpClient.o BufferedSocket.o 


%.o: %.c 
	gcc $(CFLAGS)  -c  $< -o $@ 

%.o: %.cpp
	g++ $(GPP_FLAGS)  -c -o $@ $< 



tcpServerTest:$(OBJECTS_SERVER)
	g++ $(OBJECTS_SERVER) -pthread -o tcpServerTest 


proxyClient: $(OBJECTS_PROXY_CLIENT)
	g++ $(OBJECTS_PROXY_CLIENT) -pthread -o proxyClient

proxyServer: $(OBJECTS_PROXY_SERVER)
	g++ $(OBJECTS_PROXY_SERVER) -pthread -o proxyServer

clean:
	rm $(OBJECTS_SERVER) tcpServerTest
