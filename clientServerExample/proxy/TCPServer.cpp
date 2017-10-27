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


#include "TCPServer.h"


void printError(const char* msg)
{
   cout << msg << endl;
}


TCPServer::TCPServer(uint16_t portNumber, 
                     bool onlyLocally,
                     void* (*workerProcedure)(void*)):_portNumber(portNumber),
                                                       _onlyLocally(onlyLocally),
                                                       _acceptPort(-1),
                                                       _workerProcedure(workerProcedure)
{
}


bool TCPServer::createServerSocket()
{

/* create a server socket which can be used for accept() calls */
    
    struct sockaddr_in name;

    _acceptPort = socket(PF_INET, SOCK_STREAM, 0);
    if (_acceptPort == -1)
    {
       printError("socket");
       return false;
    }

    int reuse = 1;
    if (setsockopt(_acceptPort, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
    {
        printError("setsockopt(SO_REUSEADDR) failed");
        return false;
    }


    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(_portNumber);
    if( _onlyLocally )
    {
       name.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    else
    {
       name.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    if (bind(_acceptPort, (struct sockaddr *)&name, sizeof(name)) < 0)
    {
       printError("bind");
       return false;
    }
    if (listen(_acceptPort, 5) < 0)
    {
        printError("listen");
        return false;
    }
    return true;
}

void TCPServer::getClientSocket(int& socket)
{
   _workQueue.getWork(socket);
}

/* start all the worker threads */
void TCPServer::startWorkerThreads()
{
   for(uint32_t i=0; i < _numberOfWorkers; i++)
   {
      pthread_create(&_workers[i],NULL,_workerProcedure,this);
   }
}

void TCPServer::acceptLoop()
{
    if( _acceptPort <= 0)
    {
       cerr << "accept port not created " << endl;
       return;
    }
    startWorkerThreads();

    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);

       
    while (1)//accept() loop
    {
      client_sock = accept(_acceptPort,
                           (struct sockaddr *)&client_name,
                           &client_name_len);    
      _workQueue.insertWork(client_sock);
      

      if (client_sock == -1)
      {
         printError("accept failed");
      }
    }
    close(_acceptPort);//currently unreachable
}

