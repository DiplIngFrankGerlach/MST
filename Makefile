# Example Makefile for MST
#
# 

VPATH:=./aes

CFLAGS= -g -Wall
#For production: CFLAGS= -O2 -Wall
GPP_FLAGS = -g -Wall

OBJECTS = aes.o SampleAndTesting.o UnitTest.o

OBJECTS_EXAMPLE = aes.o Example1.o

%.o: %.c
#	g++ $(GPP_FLAGS) -I./aes -c -o $@ $< 
	gcc $(CFLAGS) -I./aes -c  $< -o $@ 

%.o: %.cpp
	g++ $(GPP_FLAGS) -I./aes -c -o $@ $< 

all: $(OBJECTS)
	g++ $(OBJECTS) -o MSTTesting

Example1: $(OBJECTS_EXAMPLE)
	g++ $(OBJECTS_EXAMPLE) -o Example1 

clean:
	rm $(OBJECTS) MSTTesting
