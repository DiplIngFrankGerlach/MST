#ifndef MST_Hash_header
#define MST_Hash_header


#include <limits.h>
#include <stdint.h>
#include "DaviesMeyerHash.h"
#include "Util.h"
#include "RandomSource.h"

/* Class for generating and checking the AuthPlaintext PDU(Protocol Data Unit). 
   This class will generate the Authplaintext PDU and also check a given PDU
   for correctness. Finally it extracts the plaintext from a correctly HMAC-ed
   PDU.
   Please also refer to Protocol.txt for details about this PDU.
*/
class MST_Hash
{
   uint8_t* _outputBuffer; 
   uint32_t _outputBufferSize;
public:
   MST_Hash():_outputBuffer(NULL),_outputBufferSize(0)
   {
   }
   
   /* Create the AuthPlaintext PDU 
      The call will return a buffer which is valid until the next call to a method
      of this class. The buffer is managed by this class.
   */
   bool createAuthPlaintext(uint8_t* sourcePlaintext,//the plaintext
                            uint32_t length,         //length of plaintext
                            uint8_t** outputBuffer,  //typically a reference to a pointer
                            uint32_t* outputSize)    //typically a reference to a uint32_t 
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

   /* check the AuthPlaintext PDU and extract the plaintext of the HMAC is correct.
      Return true for a correct HMAC.
      The call will return a buffer which is valid until the next call to a method
      of this class and will be managed by this class.
   */
   bool checkAndExtractPlaintext(uint8_t*   authPlaintext,   //AuthPlainText PDU
                                 uint32_t   length,          //length of PDU
                                 uint8_t**  plaintext,       //typically a reference to a pointer
                                 uint32_t*  lengthPlaintext )//typically a reference to a uint32_t
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


/* This class realizes the MST protocol and provides all the high level method
   calls for generating all PDUs(Protocol Data Unit) of the protocol.
   It can be used for communication over all kinds of transport protocols such
   as TCP/IP, RS232, ATM, Datex-P, CAN or the like.

   Each of the two communication partners need one instance of this class in order
   to set up a secured bidirectional communications channel.

   Please refer to Protocol.txt and WhyMST.html for a conceptual description of 
   the MST protocol.
*/ 
class MST_Endpoint
{
   uint8_t _sharedSecret[16];      //the secret key shared by the communication partners
   uint8_t _maskCounterOwn[16];    //the MaskingCounter of this endpoint
   uint8_t _maskCounterPartner[16];//the MaskingCounter of the other endpoint


   unsigned int _aesSchedule[60];  //AES cipher internal state (derived from _sharedSecret)

   bool _maskCounterExchangeGenerated;        //internal state for consistency
   bool _partnerMaskCounterExchangeDecrypted; //internal state for consistency

   uint8_t* _buffer; 
   uint32_t _bufferSize;

   MST_Hash _mstHash;
	
   bool ensureBufferSize(uint32_t sz)
   {
      if(_bufferSize < sz )
      {
         _bufferSize = sz;
         delete[] _buffer;
         _buffer = new uint8_t[_bufferSize];
         if( _buffer == NULL )
         {
            return false;
         }
      }
      return true;
   }

   
 
public:
   static void incrementMC(uint8_t* counter)
   {
       bool carry(false);
       uint8_t i=0;
       do
       {
          carry = counter[i] == 255;
          counter[i++]++;
       }
       while(carry && (i < 16));
   }


   MST_Endpoint(const uint8_t* sharedSecret):_buffer(NULL),_bufferSize(0)
   {
        memcpy(_sharedSecret,sharedSecret,16);
        RandomSource rs;
        rs.getRandomNumber128(_maskCounterOwn);
        //_state = PhaseSetup;

        aes_key_setup(_sharedSecret,_aesSchedule,128);
        _maskCounterExchangeGenerated = false;
        _partnerMaskCounterExchangeDecrypted = false;
   }
   
   /* generate the 16 octet MaskCounterExchange PDU 
      The caller must provide an 16 octet sized buffer
   */
   void generateMaskCounterExchange(uint8_t* maskCounterExchange)
   {
      aes_encrypt(_maskCounterOwn,maskCounterExchange,_aesSchedule,128);
      _maskCounterExchangeGenerated = true;
   }

   void decryptMaskCounterExchange(uint8_t* encryptedMaskCounterPartner)
   {
      aes_decrypt(encryptedMaskCounterPartner,_maskCounterPartner,_aesSchedule,128);
      _partnerMaskCounterExchangeDecrypted = true;
   }

   bool decryptSecureMessage(uint8_t* securedMessage, 
                             uint32_t lengthSM,
                             uint8_t** plaintext,
                             uint32_t* lengthPlaintext)
   {
      if( !_maskCounterExchangeGenerated || !_partnerMaskCounterExchangeDecrypted )  
      {
         return false;
      }
      if( (lengthSM < 32) || ((lengthSM & 0xF) != 0 ) )
      {
         return false;
      }
      if( !ensureBufferSize(lengthSM) )
      {
         return false;
      }
      for(uint32_t i=0; i < lengthSM; i+=16)
      {
         aes_decrypt(securedMessage+i,_buffer+i,_aesSchedule,128);
         //remove masking
         uint8_t* masked = _buffer+i;
         for(uint8_t j=0; j < 16; j++)
         {
            masked[j] ^= _maskCounterPartner[j];
         }
         incrementMC(_maskCounterPartner);
      } 
      if( _mstHash.checkAndExtractPlaintext(_buffer,lengthSM,plaintext,lengthPlaintext) )
      {
         return true;
      }
      return false;
   }   

   bool encryptToSecureMessage(uint8_t* plaintext,
                               uint32_t length,
                               uint8_t** securedMessage,
                               uint32_t* lengthSM)
   {
       if( !_maskCounterExchangeGenerated || !_partnerMaskCounterExchangeDecrypted )  
       {
          return false;
       }
        
       if( _mstHash.createAuthPlaintext(plaintext,length,securedMessage,lengthSM) )
       {
	      for(uint32_t i=0; i < (*lengthSM); i+=16)
	      {
		 //apply masking
		 uint8_t encrypted[16];
		 uint8_t* masked = (*securedMessage)+i;
		 for(uint8_t j=0; j < 16; j++)
		 {
		    masked[j] ^= _maskCounterOwn[j];
		 }
		 incrementMC(_maskCounterOwn);
		 aes_encrypt(masked,encrypted,_aesSchedule,128);
		 memcpy((*securedMessage)+i,encrypted,16);
		 
	      }
              return true; 
       }
       return false;
   }

   
};


#endif



