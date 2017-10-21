/*********************************************************************************
* A TCP/IP based, secure socket communications class, using the MST crypto
* library.
*
* Implements the MST_TCP protocol
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/

#ifndef MST_SOCKET_HDR
#define MST_SOCKET_HDR

#include "Protocol.h"
#include "BufferedSocket.h"


#define MAX_PLAINTEXT_TRANSACTION_SIZE 10000000

class MST_Socket
{
   MST_Endpoint *_mstEP;
   BufferedSocket _bufSocket;
   string _hostname;
   uint16_t _port;
   uint32_t _partnerNumber;
   
   uint8_t* _readBuffer;
   uint32_t _readBufferSize;

   void ensureReadBufferSize(uint32_t size);
public:
   //the client constructor
   MST_Socket(uint8_t* sharedKey, string hostname,uint16_t port,uint32_t partnerNumber);
   

   //the server constructor
   MST_Socket(int socket);

   bool connectAndStartup();

   bool write(uint8_t* buffer, uint32_t length);

   bool read(uint8_t** buffer, uint32_t* length);

   ~MST_Socket();
};


#endif
