#ifndef DAVIES_MEYER_HASH
#define DAVIES_MEYER_HASH
#include <memory.h>
#include "Util.h"
#include "aes.h"

#define HashWordSize 16
#define HashWordSizeBits (HashWordSize * 8)

class DM_Hash
{
public:

   /* hash a multiple of 16 octets, output must be 16 octet buffer */
   static bool hash(const uint8_t* input, uint32_t length, uint8_t* output)
   {
      if( (length == 0) || ((length & (HashWordSize-1)) != 0) )
      {
         return false;
      }
      unsigned int schedule[60];
      uint8_t state1[HashWordSize];
      uint8_t state2[HashWordSize];
      uint8_t* stateInPtr  = state1;
      uint8_t* stateOutPtr = state2;
      memset(stateInPtr,0,HashWordSize);
      for(uint32_t i=0; i < length;i += 16)
      {
          aes_key_setup(input+i,
                        schedule, 
                        HashWordSizeBits);

          aes_encrypt(stateInPtr, 
                      stateOutPtr,
                      schedule,  
                      HashWordSizeBits) ;
          uint8_t* temp = stateInPtr;
          stateInPtr = stateOutPtr;
          stateOutPtr = temp;
      }
      memcpy(output,stateInPtr,HashWordSize);//stateInPtr is in fact the output here
      return true;
   }

    
};

#endif



