#ifndef MST_Hash_header
#define MST_Hash_header


#include <limits.h>
#include <stdint.h>
#include "DaviesMeyerHash.h"
#include "Util.h"

/* Class for generating and checking the AuthPlaintext PDU. Also used to extract the plaintext
   again.
*/
class MST_Hash
{
   uint8_t* _outputBuffer; 
   uint32_t _outputBufferSize;
public:
   MST_Hash():_outputBuffer(NULL),_outputBufferSize(0)
   {
   }
   
   bool createAuthPlaintext(uint8_t* sourcePlaintext,
                            uint32_t length,
                            uint8_t** outputBuffer,
                            uint32_t* outputSize)
   {
        if( (length == 0) || (length > (UINT_MAX-32) ) )
        {
          return false;
        }
        if( _outputBufferSize < (length+32) )
        {
           delete[] _outputBuffer;
           
           _outputBufferSize = length + 4 + 32;
           _outputBuffer = new uint8_t[_outputBufferSize];
           if( _outputBuffer == NULL )
           {
              return false;
           }
        }
        Util::int2Octets(length,_outputBuffer);
        memcpy(_outputBuffer+4,sourcePlaintext,length);
        uint32_t pos;
        for( pos = length+4; (pos & 0xF) != 0; pos++)
        {
           _outputBuffer[pos]=0;
        }
         
        if( DM_Hash::hash(_outputBuffer,pos,_outputBuffer+pos) == false)
        {
           return false;
        }
        *outputBuffer = _outputBuffer;
        *outputSize = pos + 16;
        return true;
   }

   bool checkAndExtractPlaintext(uint8_t*   authPlaintext,
                                 uint32_t   length,
                                 uint8_t**  plaintext,
                                 uint32_t*  lengthPlaintext )
   {
      *plaintext = NULL;
      if( length < 32 )
      {
        return false;
      }
      uint8_t computedHash[16];
      if(DM_Hash::hash(authPlaintext,length-16,computedHash) )
      {
         if( memcmp(computedHash,authPlaintext+length-16,16) == 0 )
         {
            Util::octets2Int(authPlaintext,lengthPlaintext);
            if( (*lengthPlaintext) < (length-16) )
            {
               *plaintext = authPlaintext+4;
               return true;
            }            
         }
      } 
      return false;
   }

   ~MST_Hash()
   {
      delete[] _outputBuffer;
      _outputBufferSize = 0;
   }
   
   
};

#endif



