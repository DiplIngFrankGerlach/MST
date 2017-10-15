/* Einfacher und sicherer Webserver, in C++.

   Erweiterbar durch benutzerdefinierten C++ Code.

   (C) Frank Gerlach 2017, frankgerlach.tai@gmx.de

   Kommerzielle Nutzung erfordert eine kostenpflichtige Lizenz.

*/




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
 

using namespace std;

 

ThreadWorkQueue<int> __workQueue; // a queue of socket handles to process by Worker Threads
const int __numberOfWorkers = 10;
pthread_t  __workers[__numberOfWorkers]; //thread data structures


void fatalError(const char* errorMessage)
{
   perror(errorMessage);
   exit(-1);
}


class SocketSchliesserHelfer
{
   int m_socket;
public:
   SocketSchliesserHelfer(int socket): m_socket(socket)
   {
   }
   ~SocketSchliesserHelfer()
   {
      close(m_socket);
   }
};

void* workerProcedure(void*)
{
   int socket;
   __workQueue.getWork(socket);
   
   const char* msg= "hello from worker";
   write(socket,msg,strlen(msg));

   close(socket);
   return NULL;
}


void startWorkerThreads()
{
   for(uint32_t i=0; i < __numberOfWorkers; i++)
   {
      pthread_create(&__workers[i],NULL,workerProcedure,NULL);
   }
}




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

    while (1)
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
