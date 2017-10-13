# Example Makefile for MST
#
# 

VPATH:=./aes

CFLAGS= -g -Wall
#For production: CFLAGS= -O2 -Wall
GPP_FLAGS = -g -Wall

OBJECTS = aes.o SampleAndTesting.o UnitTest.o

%.o: %.c
#	g++ $(GPP_FLAGS) -I./aes -c -o $@ $< 
	gcc $(CFLAGS) -I./aes -c  $< -o $@ 

%.o: %.cpp
	g++ $(GPP_FLAGS) -I./aes -c -o $@ $< 

all: $(OBJECTS)
	g++ $(OBJECTS) -o MSTTesting

clean:
	rm $(OBJECTS) MSTTesting
