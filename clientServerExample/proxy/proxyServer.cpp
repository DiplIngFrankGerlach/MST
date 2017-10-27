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

string __targetDNSname;
uint16_t __targetPortNumber;

struct swContext
{
     ::std::shared_ptr<BufferedSocket> _serverConnection;//plaintext connection to local server
     ::std::shared_ptr<MST_Socket> _securedConnection; //MST secured connection to remote proxy Client
     pthread_t _thread;
};

void* serverWriter(void* arg)
{
    swContext* ctx = (swContext*) arg;
    
    do
    {
       uint8_t* buffer(NULL);
       uint32_t bufLength(0);
       if( ctx->_securedConnection->read(&buffer,&bufLength) )
       {
          if( !ctx->_serverConnection->write(buffer,bufLength) || !ctx->_serverConnection->flush())
          {
              cout << "server connection broken " << endl;
              delete ctx;
              return NULL;
          } 
       }
       else
       {
           cout << "secured connection broken " << endl;
           delete ctx;
           return NULL;
       } 
    }
    while(true);
}


void* serverWorker(void* arg)
{
   TCPServer* tcpServer = (TCPServer*)arg;
   while(true)
   {
      int socket;
      tcpServer->getClientSocket(socket);
      
      shared_ptr<MST_Socket> securedConnection(new MST_Socket(socket));

      if(securedConnection->connectAndStartup() )
      {
          ::std::shared_ptr<BufferedSocket> bSocket(new BufferedSocket);
          if( bSocket->connect(__targetDNSname,__targetPortNumber) )
          {
             //start writer thread
             swContext* ctx = new swContext;
             ctx->_serverConnection = bSocket;
             ctx->_securedConnection = securedConnection;

             pthread_create(&(ctx->_thread),NULL,serverWriter,ctx);


             while(true)
             {
                const int bufferSz = 2000;
                char buffer[bufferSz];
                int ret = ::read(bSocket->getRawSocket(),buffer,bufferSz);
                if( ret > 0 )
                { 
                   if( !securedConnection->write((uint8_t*)buffer,(uint32_t)ret) ) 
                   {
                        cout << "secured Connection broken" << endl;
                        close(socket);
                        return NULL;     
                   }
                }
                else
                {
                  cout << "server Connection broken(read)" << endl;
                  close(socket);
                  return NULL;
                }
             }
          }
      }   
       
   }//end while(true)
    
   return NULL;
}




int main(int argc,char** argv)
{
   uint16_t listenPort;
   if( argc == 4)
   {
       sscanf (argv[1], "%" SCNd16, &listenPort);
       __targetDNSname = argv[2];
       sscanf (argv[3], "%" SCNd16, &__targetPortNumber);
   }
   else
   {
     cerr << "usage: proxyServer <listenPort> <DNSnameOfTargetServer> <portOfTargetServer>";
     return -1;
   }
   if( __keyReader.readPresharedKeys() == false )
   {
      cerr << "could not read preshared Keys" << endl;
      return -1;
   }
   signal(SIGPIPE, SIG_IGN);//igrnore SIGPIPE Signals

   TCPServer ts(listenPort,true,serverWorker);
   if( ts.createServerSocket() )
   {
        cout << "running accept loop" << endl;
        ts.acceptLoop();
   }
   else cout << "failure to create server" << endl;
   return 0;
}
