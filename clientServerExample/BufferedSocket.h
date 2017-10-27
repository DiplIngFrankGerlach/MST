/*********************************************************************************
* BufferedSocket, a class/module for simple TCP communications
*
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/

#ifndef BUFFERED_SOCKET_HDR
#define BUFFERED_SOCKET_HDR

#include <unistd.h>
#include <iostream>
#include <memory.h>

using namespace std;

/* A class encapsulating common TCP socket operations such as 
   + create socket, look up IP address, connect
   + buffer read and write operations for efficiency
   + close buffer when quitting context (RAII pattern)
*/
class BufferedSocket
{ 

   int _socket;
   //write buffer
   uint8_t* _buffer;
   uint32_t _bufferCapacity;
   uint32_t _bufferUsed;

   //read buffer
   uint8_t* _readBuffer;
   uint32_t _readBufferCapacity;
   uint32_t _readBufferValid;
   uint32_t _readPtr;


   bool setCapacity(uint32_t newCap)
   {
         uint8_t* newBuffer = new uint8_t[newCap];
         if( newCap > _bufferUsed)
         {
            memcpy(newBuffer,_buffer,_bufferUsed);    
         }
         else
         {
            if( !flush() )
            {
              return false;
            }
         }
         delete[] _buffer;
         _buffer = newBuffer;
         _bufferCapacity = newCap;
         return true;
   }


   
public:
   /* generate instance, but defer socket creation to connect() */
   BufferedSocket(): _socket(-1),
                     _buffer(NULL),
                     _bufferCapacity(0),
                     _bufferUsed(0),
                     _readBuffer(NULL),
                     _readBufferCapacity(0),
                     _readBufferValid(0),
                     _readPtr(0)
   {
       setCapacity(1600);

       _readBufferCapacity = 1600;
       _readBuffer = new uint8_t[_readBufferCapacity];
   }

   /* generate instance with an existing, connected socket. This will be typically used for server-side programs.

      NOTE: This class will do the closing of the socket handle !
   */
   BufferedSocket(   int socket,
                     uint32_t sendBufferSize=1600,
                     uint32_t recvBufferSize=1600):
                              _socket(socket),
                              _buffer(NULL),
                              _bufferCapacity(0),
                              _bufferUsed(0),
                              _readBuffer(NULL),
                              _readBufferCapacity(0),
                              _readBufferValid(0),
                              _readPtr(0)
   {
       setCapacity(sendBufferSize);

       _readBufferCapacity = recvBufferSize;
       _readBuffer = new uint8_t[_readBufferCapacity];
   }

   /* connect to a server given by DNS name */
   bool connect(const string& hostname,uint16_t portNum);

   int getRawSocket()
   {
      return _socket;
   }


   ~BufferedSocket()
   {
      delete[] _buffer;
      _buffer = NULL;
      delete[] _readBuffer;
      _readBuffer = NULL;

      if( _socket > 0 )
      {
        ::close(_socket);
        _socket = -1;
      }
   }

   /* write/send a number of octets.
      Note: To ensure data being sent to the communications partner, call flush()
   */
   bool write(uint8_t* data,uint32_t length)
   {
       uint32_t writePtr=0;
       while(writePtr < length)
       {
           if( _bufferUsed == _bufferCapacity )
           {
               if( !flush() )
               {
                   return false;
               }
           }
           while( (writePtr < length) && (_bufferUsed < _bufferCapacity) )
           {
              _buffer[_bufferUsed++] = data[writePtr++];
           }
       }
       return true;
   }

   /* read octets, which have been sent by the partner. 
      The call will block until numOctets have been read into buffer. 
      Internally, the read buffer might contain more octets
   */
   bool read(uint8_t* buffer,uint32_t numOctets)              
   {
       uint32_t octetsRead(0);
       while(octetsRead < numOctets)
       {
          while( (_readPtr < _readBufferValid) && (octetsRead < numOctets) )
          {
             buffer[octetsRead++] = _readBuffer[_readPtr++];
          }
          if( (octetsRead != numOctets) && ( _readPtr == _readBufferValid) )
          {
             int ret = ::read(_socket,_readBuffer,_readBufferCapacity);
             if( ret <= 0 )
             {
                return false;
             }
             else
             {
                _readBufferValid = ret;
                _readPtr = 0;
             }
          }
       }
       return true;
   }

   /* flush the write buffer to the communications partner */
   bool flush()
   {
       uint32_t sent(0);
       while(sent < _bufferUsed )
       {
          //cout << "write() " << (_bufferUsed-sent) << endl;
          int ret = ::write(_socket,_buffer+sent,_bufferUsed-sent);
          if( ret <= 0)
          {
             return false;
          }
          sent += ret;    
       }
       _bufferUsed = 0;
       return true;
   }

   
};

bool readInteger32(BufferedSocket& bSocket,uint32_t* output);

bool writeInteger32(BufferedSocket& bSocket,uint32_t number);

#endif
