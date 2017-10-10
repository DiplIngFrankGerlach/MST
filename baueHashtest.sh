#g++ -g -Wall -pedantic -Wextra HashTest.cpp  -I ../crypto-algorithms ../crypto-algorithms/aes.c \
#    -o hashTest

g++ -O2 -Wall -pedantic -Wextra HashTest.cpp  -I ../crypto-algorithms ../crypto-algorithms/aes.c \
   -o hashTest
