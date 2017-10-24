/*********************************************************************************
* A client program demonstrating secure communications via MST.
*
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/


#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netinet/tcp.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include "MST_Socket.h"


void die(const char *errorMessage)
{
  perror(errorMessage);
  exit(1);
}

int main(int argc, char *argv[])
{
    
    string hn("127.0.0.1");

    uint8_t sharedKey[] = {0x9f,0x51,0xcf,0xc5,0xfd,0x1b,0x2a,0x17,0x57,0x9e,0x61,0x78,0xf8,0x5c,0x02,0xb3};
    
    MST_Socket mstSock(sharedKey,hn,1234,1001);
    //cout << "C1" << endl;
    if( mstSock.connectAndStartup() )
    {
       //cout << "C2" << endl;
       string message("I am the client, hello");
       if(mstSock.write( (uint8_t*)message.c_str(),message.length()+1) ) 
       {
          //cout << "C3" << endl;
          uint8_t* reply(NULL);
          uint32_t replyLength(0);
          if( mstSock.read(&reply,&replyLength) )
          {
             //cout << "C4" << endl;
             cout << ((const char*)reply) << endl;
          }
       }
    }

    //close(sock);
     
}
