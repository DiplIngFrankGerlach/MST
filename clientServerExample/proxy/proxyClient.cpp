/*********************************************************************************
* Secure Proxy, Client Side Server
*
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/


#include <string.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>

extern "C" {
#include <pthread.h>
}

#include <sys/wait.h>
#include <stdlib.h>
#include <iostream>
#include "TCPServer.h"
#include "MST_Socket.h"
#include "TCPUtil.h"
#include "KeyReader.h"

using namespace std;


string __serverDNSName;
uint16_t __serverPort;

uint8_t __presharedKey[16];
uint32_t __partnerNumber;

struct cwContext
{
     int _clientSocket;
     ::std::shared_ptr<MST_Socket> _securedConnection;
     pthread_t _thread;
};

void* clientWriter(void* arg)
{
    cwContext* ctx = (cwContext*) arg;
    
    do
    {
       const int bufSZ = 3000;
       char buffer[bufSZ];
       int ret = ::read(ctx->_clientSocket,buffer,bufSZ);
       if( ret <= 0 )
       {
          close(ctx->_clientSocket);
          delete ctx;
          return NULL;//socket connection broken, exit thread
       }
       else
       {
           if( !ctx->_securedConnection->write((uint8_t*)buffer,(uint32_t)ret) )
           {
              close(ctx->_clientSocket);
              delete ctx;
              return NULL; //secured connection broken, exit thread
           }
       }
    }
    while(true);
}


void* proxyWorker(void* arg)
{
   TCPServer* tcpServer = (TCPServer*)arg;
   while(true)
   {
       int socket;
       tcpServer->getClientSocket(socket);
      
       ::std::shared_ptr<MST_Socket> mstSock(new MST_Socket(__presharedKey,__serverDNSName,__serverPort,__partnerNumber));
       //cout << "C1" << endl;
       if( mstSock->connectAndStartup() )
       {
          //cout << "C2" << endl;
         
          cwContext* ctx = new cwContext;
          ctx->_clientSocket = socket;
          ctx->_securedConnection = mstSock;
          
          pthread_create(&(ctx->_thread),NULL,clientWriter,ctx);

          bool socketOK(true);
          while( socketOK )
          {
             uint8_t* reply(NULL);
             uint32_t replyLength(0);
             if( mstSock->read(&reply,&replyLength) )
             {
                  if( !writeSocketFully(ctx->_clientSocket,reply,replyLength) )
                  {
                     close(socket);
                     return NULL;
                  }
             }
             else
             {
                  close(socket);
                  return NULL;
             }
          }
       } 
       
   }//end while(true)
    
   return NULL;
}




int main(int argc, char** argv)
{
   if( argc == 5 )
   {
      __serverDNSName = argv[1];
      sscanf (argv[2], "%" SCNd16, &__serverPort);
      sscanf (argv[3], "%" SCNd32, &__partnerNumber);


      bool success(false);
      KeyReader kr;
      if( kr.loadOneKeyToBuffer(argv[4]) )
      {
          if( kr.readPSKey(__presharedKey) )
          {
             success = true;
          }
      }
      if(!success)
      {
         cerr << "key not parseable" << endl;
         return -1;
      }
      
   }
   else
   {
     cerr << "usage: proxyClient <DNSserverName> <serverPort> <partnerNumber> <key>" << endl;
     cerr << "key must be 16 hexadecimal octets separated by '-'" << endl;
     return -1;
   }
   signal(SIGPIPE, SIG_IGN);//igrnore SIGPIPE Signals

   TCPServer ts(2000,true,proxyWorker);
   if( ts.createServerSocket() )
   {
        cout << "running accept loop" << endl;
        ts.acceptLoop();
   }
   else cout << "failure to create server" << endl;
   return 0;
}
