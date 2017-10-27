#include <string.h>
#include <unistd.h>
#include <iostream>
#include "TCPServer.h"

using namespace std;

void* helloWorker(void* arg)
{
   TCPServer* tcpServer = (TCPServer*)arg;
   while(true)
   {
      int socket;
      tcpServer->getClientSocket(socket);
      
      const char* msg ="hello from server";
      write(socket, msg,strlen(msg)+1);
      close(socket);  
       
   }//end while(true)
    
   return NULL;
}




int main()
{
   TCPServer ts(1235,false,helloWorker);
   if( ts.createServerSocket() )
   {
        cout << "running accept loop" << endl;
        ts.acceptLoop();
   }
   else cout << "failure to create server" << endl;
   return 0;
}
