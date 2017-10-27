#ifndef TCP_SERVER_HDR
#define TCP_SERVER_HDR

#include <stdint.h>
#include <pthread.h>
#include "ThreadWorkQueue.h"

class TCPServer
{
   uint16_t _portNumber;
   bool _onlyLocally;
   int _acceptPort;
   ThreadWorkQueue<int> _workQueue;
   
   static const int _numberOfWorkers = 10;//number of worker threads
   pthread_t  _workers[_numberOfWorkers]; //thread data structures

   void* (*_workerProcedure)(void*);

   void startWorkerThreads();

public:
   TCPServer(uint16_t portNumber, bool onlyLocally,void* (*workerProcedure)(void*));

   bool createServerSocket();

   void acceptLoop();

   void getClientSocket(int& socket);

   //todo: ~TCPServer

 
};

#endif
