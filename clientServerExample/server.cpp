
/*********************************************************************************
* A simple server, demonstrating the use of the MST crypto library 
*
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/




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

extern "C" {
#include <pthread.h>
}

#include <sys/wait.h>
#include <stdlib.h>
#include <iostream>
 
#include "ThreadWorkQueue.h"
#include "Protocol.h"
#include "BufferedSocket.h"
#include "Util.h"

using namespace std;

 

ThreadWorkQueue<int> __workQueue; // a queue of socket handles to process by Worker Threads
const int __numberOfWorkers = 10;//number of worker threads
pthread_t  __workers[__numberOfWorkers]; //thread data structures


/* print a fatal error message and stop the program */
void fatalError(const char* errorMessage)
{
   perror(errorMessage);
   exit(-1);
}


/* The thread worker procedure, which receives work (connected socket) via the __workQueue.
   It demonstrates how to set up an MST Endpoint and how to transfer a message securely from
   client to server.
   Note that the lookup of shared key based on the partnerNumber is not yet implemented
*/
void* workerProcedure(void*)
{
   while(true)
   {
      int socket;
      __workQueue.getWork(socket);
      
      BufferedSocket bSocket(socket);

      uint32_t partnerNumber;
      if( readInteger32(bSocket,&partnerNumber) )
      {

         cout << "partnerNumber: " << partnerNumber << endl;
        

         uint8_t sharedKey[] = {0x9f,0x51,0xcf,0xc5,0xfd,0x1b,0x2a,0x17,0x57,0x9e,0x61,0x78,0xf8,0x5c,0x02,0xb3};
         MST_Endpoint ep(sharedKey);

         uint8_t mce[16];

         if( bSocket.read(mce,16) )
         {
            cout << "X" << endl;
            ep.decryptMaskCounterExchange(mce);
            uint32_t l;

            if( readInteger32(bSocket,&l) && (l < 100000))
            {
               cout << "X" << endl;
               uint8_t* securedMessage = new uint8_t[l];
               
               bSocket.read(securedMessage,l);
               
               uint8_t* plaintext;
               uint32_t ptLength;
               if( ep.decryptSecureMessage(securedMessage,l,&plaintext,&ptLength) )
               {
                  cout << "plaintext: " << ((char*)plaintext) << endl;
               }        
            }
         }
      }
         

       
  }//end while(true)
    
   return NULL;
}

/* start all the worker threads */
void startWorkerThreads()
{
   for(uint32_t i=0; i < __numberOfWorkers; i++)
   {
      pthread_create(&__workers[i],NULL,workerProcedure,NULL);
   }
}



/* create a server socket which can be used for accept() calls */
int createSocketAndListen(u_short *port)
{
    int acceptPort = 0;
    struct sockaddr_in name;

    acceptPort = socket(PF_INET, SOCK_STREAM, 0);
    if (acceptPort == -1)
    {
      fatalError("socket");
    }

    int reuse = 1;
    if (setsockopt(acceptPort, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
    {
        fatalError("setsockopt(SO_REUSEADDR) failed");
    }


    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(acceptPort, (struct sockaddr *)&name, sizeof(name)) < 0)
    {
       fatalError("bind");
    }
    if (*port == 0)
    {
        socklen_t namelen = sizeof(name);
        if (getsockname(acceptPort, (struct sockaddr *)&name, &namelen) == -1)
        {
            fatalError("getsockname");
        }
        *port = ntohs(name.sin_port);
    }
    if (listen(acceptPort, 5) < 0)
    {
        fatalError("listen");
    }

    return acceptPort;
}


  
/* start up the server */
int main(void)
{
     
    startWorkerThreads();

    int server_sock = -1;
    u_short port = 1234;
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);


    server_sock = createSocketAndListen(&port);
    printf("httpd running on port %d\n", port);

    while (1)//accept() loop
    {
      client_sock = accept(server_sock,
                          (struct sockaddr *)&client_name,
                          &client_name_len);

      
      __workQueue.insertWork(client_sock);
      

      if (client_sock == -1)
      {
        perror("accept failed");
      }

       
    }

    close(server_sock);

    return(0);
}
