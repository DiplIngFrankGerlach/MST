/*********************************************************************************
* BufferedSocket, a class/module for simple TCP communications
*
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/


#include <stdint.h>
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netinet/tcp.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include <netdb.h>

#include "BufferedSocket.h"
#include "Util.h"


bool BufferedSocket::connect(const string& hostname,uint16_t portNum)
{
   struct sockaddr_in servAddr; /* Echo server address */
 
   struct hostent* hostEntry = gethostbyname(hostname.c_str());
   if( hostEntry == NULL)
   {
      return false;
   }
   if( hostEntry->h_length != 4 )//just IPv4 currently - IPv6 available on request
   {
      return false;
   }
   in_addr* pIA = (in_addr*)hostEntry->h_addr_list[0];
   if( pIA != NULL )
   {
      //Util::dumpHex( (uint8_t*)pIA,4);
   }
   else return false;

   /* Create a reliable, stream socket using TCP */
   if ((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
   {
    return false;
   }

   /* Construct the server address structure */
   memset(&servAddr, 0, sizeof(servAddr));     /* Zero out structure */
   servAddr.sin_family      = AF_INET;             /* Internet address family */
   memcpy(&servAddr.sin_addr.s_addr,pIA,4);   /* Server IP address */
   servAddr.sin_port        = htons(portNum); /* Server port */

   /* Establish the connection to the server */
   if (::connect(_socket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
   {
   return false;
   }
   return true;
}

bool readInteger32(BufferedSocket& bSocket,uint32_t* output)
{
      uint8_t number[4];//numeric id of communications partner
      if( bSocket.read(number,4) )
      {
         Util::octets2Int(number,output);
         return true;
      }
      return false;
}

bool writeInteger32(BufferedSocket& bSocket,uint32_t number)
{
   uint8_t numberBuffer[4];
   Util::int2Octets(number,numberBuffer);
   if( bSocket.write(numberBuffer,4 ) )
   {
     return true;
   }
   return false;
}

