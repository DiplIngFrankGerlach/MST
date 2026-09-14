/*********************************************************************************
* Minimal Secure Transport Library
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2026 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/


#ifndef DAVIES_MEYER_HASH
#define DAVIES_MEYER_HASH
#include <memory.h>
#include "Util.h"
#include "aes.h"

#define HashWordSize 32
#define HashWordSizeBits (HashWordSize * 8)


/* Implement the Davies-Meyer Hash Function based on AES */
class DM_Hash
{
public:

   /* hash a multiple of 32 octets, output must be 32 octet buffer */
   static bool hash(const uint8_t* input, uint32_t length, uint8_t* output)
   {
      //cout << "DM_Hash output: " << ((void*)output) << endl;


      if( (length == 0) || ((length & (HashWordSize-1)) != 0) )
      {
         cout << "Falsche Laenge" << endl;
         return false;
      }
      unsigned int schedule[60];
      memset(schedule,0,60*sizeof(uint32_t));
      uint8_t state1[HashWordSize];
      memset(state1,0,HashWordSize);
      uint8_t state2[HashWordSize];
      memset(state2,0,HashWordSize);
      uint8_t* stateInPtr  = state1;
      uint8_t* stateOutPtr = state2;
      for(uint32_t i=0; i < length;i += HashWordSize)
      {
          aes_key_setup(input+i,
                        schedule, 
                        HashWordSizeBits);

          aes_encrypt(stateInPtr, 
                      stateOutPtr,
                      schedule,  
                      HashWordSizeBits) ;

          aes_encrypt(stateInPtr  + (HashWordSize/2), 
                      stateOutPtr + (HashWordSize/2),
                      schedule,  
                      HashWordSizeBits) ;

          //H[i] = E(PT,H[i-1]) XOR H[i-1]
          for( uint32_t j=0; j < HashWordSize; j++)
          {
             stateOutPtr[j] ^= stateInPtr[j];
          }

          uint8_t* temp = stateInPtr;
          stateInPtr = stateOutPtr;
          stateOutPtr = temp;
      }
      //cout << "Zielpuffer output: " << ((void*)output) << endl;
      memcpy(output,stateInPtr,HashWordSize);//stateInPtr is in fact the output here
      return true;
   }    
};





#endif



