#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

//fully write a buffer to a TCP socket
bool writeSocketFully(int socket,uint8_t* buffer, uint32_t length)
{
   uint32_t written(0);
   do
   {
       int ret = ::write(socket,buffer+written,length-written);
       if( ret > 0 )
       {
          written += ret;
       }
       else
       {
          return false;
       }
   }
   while(written < length);
   return true;
}
