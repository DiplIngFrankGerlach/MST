/* A minimal transport cipher, which is simple enough to be inspected
   for correctness.
  
   Licensed under the BSD License

   Author: Dipl. Ing.(BA) Frank Gerlach, frankgerlach.taigmx.de
*/

#ifndef SECURE_MINIMAL_TRANSPORT
#define SECURE_MINIMAL_TRANSPORT

#include "DummyCipher.h"
#include "RandomSource.h"
#include "Utilities.h"

class SecureRequest
{
    DummyCipher* _dc;
public:
    //initialize with a cipher engine
    SecureRequest(DummyCipher* cipherEngine)
    {
       _dc = cipherEngine;
    }

    /* decrypt the request contained in securedMessage and return a
       pointer to the decrypted data in outputPtr.
       NOTE: securedMessage will be modified and must not be touched as long
             as outputPtr is used
    */   
    bool decrypt(char* securedMessage, 
                 uint32_t length,
                 char*& outputPtr,
                 uint32_t& outputLength)
    {
       //check for multiple length of 32 octets, the symmetric cipher width
       if( (length ==0) || (length & 31) )
       {
          return false;
       }
       const char* blockPtr = securedMessage;
       uint32_t lengthCtr = length;
       for(;lengthCtr > 0; lengthCtr-=32)
       {
          _dc->decrypt(blockPtr);
          blockPtr+=32;
       }
       //check message integrity
       MACEngine me;
       if( me.check(securedMessage,length) == false) 
       {
         return false;
       }
       outputLength = bigEndianToUint32(securedMessage);
       if( (outputLength > (length -4)) || (outputLength < (length -31)) )
       {
         return false;
       } 
       outputPtr = securedMessage + 4;
    }
};

#endif

