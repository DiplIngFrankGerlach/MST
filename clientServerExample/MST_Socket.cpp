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
#include <limits.h>
#include "MST_Socket.h"
#include "Util.h"
#include "KeyReader.h"


MST_Socket::MST_Socket(int socket):_mstEP(NULL),
                                   _bufSocket(socket),
                                   _port(USHRT_MAX),
                                   _partnerNumber(UINT_MAX),
                                   _readBuffer(NULL),
                                   _readBufferSize(0),
                                   _isServer(true)
{
}

MST_Socket::MST_Socket(uint8_t* key,
                       string hostname,
                       uint16_t port,
                       uint32_t partnerNumber):_mstEP(NULL),
                                               _hostname(hostname),
                                               _port(port),
                                               _partnerNumber(partnerNumber),
                                               _readBuffer(NULL),
                                               _readBufferSize(0),
                                               _isServer(false)
{
   memcpy(_clientKey,key,16);
}


void MST_Socket::ensureReadBufferSize(uint32_t size)
{
   if( _readBufferSize < size )
   {
     delete[] _readBuffer;
     _readBufferSize = size;
     _readBuffer = new uint8_t[_readBufferSize];
   }
}

/* performs the connect (if necessary) and the initial cryptographic handshake */
bool MST_Socket::connectAndStartup()
{
   if( !_isServer )
   {
     //client case
     if( !_bufSocket.connect(_hostname,_port) )
     {
        //cout << "F1" << endl;  
        return false;
     } 
     _mstEP = new MST_Endpoint(_clientKey); 
   }
   
   //cout << "S1" << endl;
   if( !(writeInteger32(_bufSocket,_partnerNumber) && _bufSocket.flush() ) )
   {
     return false;
   }

   //cout << "S2" << endl;

   //read partner number
   uint32_t partnerNumberRead;
   if( readInteger32(_bufSocket,&partnerNumberRead) )
   {
      if( _isServer )
      {
         uint8_t* presharedKey(NULL);
         if( __keyReader.getKeyForPartnerID(partnerNumberRead,&presharedKey) )
         {         
            _mstEP = new MST_Endpoint(presharedKey);
         }
         else return false;
      }

      //cout << "S3" << endl;
      //exchange Mask Counters
      uint8_t mce[16];
      _mstEP->generateMaskCounterExchange(mce);

      if( _bufSocket.write(mce,16) && _bufSocket.flush() )
      {
          //cout << "S4" << endl;
          uint8_t mcePartner[16];
          if( _bufSocket.read(mcePartner,16) )
          {
              //cout << "S5" << endl;
              _mstEP->decryptMaskCounterExchange(mcePartner);
              return true;
          }
      }
   }
   return false;
}

bool MST_Socket::write(uint8_t* buffer, uint32_t length)
{
    if( length <= MAX_PLAINTEXT_TRANSACTION_SIZE )
    {
       uint8_t* secureMessage(NULL);
       uint32_t lengthSM(0);
       if( (_mstEP != NULL) && _mstEP->encryptToSecureMessage(buffer,length,&secureMessage,&lengthSM) )
       {
            if( writeInteger32(_bufSocket,lengthSM) )
            {
               if( _bufSocket.write(secureMessage, lengthSM) && _bufSocket.flush() )
               {
                 return true;
               }
            }
       }
    }
    return false;
}

bool MST_Socket::read(uint8_t** buffer, uint32_t* length)
{
   uint32_t smLength(0);
   if( readInteger32(_bufSocket,&smLength) )
   {
       if( smLength <= (MAX_PLAINTEXT_TRANSACTION_SIZE+ 2*16) )
       {
          ensureReadBufferSize(smLength);
          if( _bufSocket.read(_readBuffer,smLength) )
          {
              if( (_mstEP != NULL ) && _mstEP->decryptSecureMessage(_readBuffer,smLength,buffer,length) )
              {
                  return true;
              }
          }
       }
   }
   return false;
     
}

MST_Socket::~MST_Socket()
{
   delete[] _readBuffer;
   delete _mstEP;
   _mstEP = NULL;
}




