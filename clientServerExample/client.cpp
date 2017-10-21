#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netinet/tcp.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include "BufferedSocket.h"
#include "Protocol.h"

void die(const char *errorMessage)
{
  perror(errorMessage);
  exit(1);
}

int main(int argc, char *argv[])
{
    
    BufferedSocket bufSock;
    //string hn("localhost");
    string hn("192.168.0.18");

    bufSock.connect(hn,1234);

    writeInteger32(bufSock,1001);//partnerNumber

    uint8_t sharedKey[] = {0x9f,0x51,0xcf,0xc5,0xfd,0x1b,0x2a,0x17,0x57,0x9e,0x61,0x78,0xf8,0x5c,0x02,0xb3};
    MST_Endpoint ep(sharedKey);

    uint8_t mce[16];
    ep.generateMaskCounterExchange(mce);

    bufSock.write(mce,16);
    bufSock.flush();

    uint8_t* secureMessage;
    uint32_t smLength;
    string plaintext("The quick brown fox jumps over the lazy dog");
    if( ep.encryptToSecureMessage((uint8_t*)(plaintext.c_str()),(uint32_t)plaintext.length(),&secureMessage, &smLength) )
    {
       writeInteger32(bufSock,smLength);
       bufSock.write(secureMessage,smLength);
       bufSock.flush();
    }


    //close(sock);
     
}
