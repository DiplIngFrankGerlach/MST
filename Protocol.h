/*********************************************************************************
* Minimal Secure Transport Library
*
* Copyright (C) 2026 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/

#ifndef MST_Hash_header
#define MST_Hash_header


#include <limits.h>
#include <stdint.h>
#include "DaviesMeyerHash.h"
#include "Util.h"
#include "RandomSource.h"
#include "GeneralUtil.h"


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

        uint32_t neededSize = length + 4 + 32 - (length % 32);
 
        if( _outputBufferSize < neededSize )
        {
           memset(_outputBuffer,0,_outputBufferSize);
           delete[] _outputBuffer;
           
           _outputBufferSize = length + 4 + 32;
           _outputBuffer = new uint8_t[_outputBufferSize];
           if( _outputBuffer == NULL )
           {
              return false;
           }
        }
        //write message length
        Util::int2Octets(length,_outputBuffer);
        memcpy(_outputBuffer+4,sourcePlaintext,length);

        //zero out the rest of the last 32 Octet word
        uint32_t pos;
        for( pos = length+4; (pos & (32-1)) != 0; pos++)
        {
           _outputBuffer[pos]=0;
        }
         
        if( DM_Hash::hash(_outputBuffer,pos,_outputBuffer+pos) == false)
        {
           return false;
        }
        *outputBuffer = _outputBuffer;
        *outputSize = pos + 32;
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
      uint8_t computedHash[32];
      if(DM_Hash::hash(authPlaintext,length-32,computedHash) )
      {
         if( timing_insensitive_memcmp(computedHash,authPlaintext+length-32,32) == 0 )
         {
            Util::octets2Int(authPlaintext,lengthPlaintext);
            if( (*lengthPlaintext) < (length - 32 ) )
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
      memset(_outputBuffer,0,_outputBufferSize);
      delete[] _outputBuffer;
      _outputBuffer = NULL;
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
   uint8_t _sharedSecret[32];      //the secret key shared by the communication partners
   uint8_t _sessionKeyOwn[32];
   uint8_t _sessionKeyOwnEncrypted[32];
   uint8_t _sessionKeyPartner[32];
   uint8_t _CounterOwn[16];    //the MaskingCounter of this endpoint
   uint8_t _CounterPartner[16];//the MaskingCounter of the other endpoint
  


   unsigned int _aesSchedule[60];  //shared master secret AES cipher internal state (derived from _sharedSecret)
   unsigned int _aesScheduleSessionKeyOwn[60]; // AES Session key state for reception 
   unsigned int _aesScheduleSessionKeyPartner[60]; // AES Session key state for sending

   bool _sessionKeySendingCreated;     
   bool _sessionKeyReceivingCreated;     
   
   uint8_t* _buffer; 
   uint32_t _bufferSize;

   MST_Hash _mstHash;
	
   bool ensureBufferSize(uint32_t sz)
   {
      if(_bufferSize < sz )
      {
         memset(_buffer,0,_bufferSize);
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
   /* method for incrementing an 128 bit counter */
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

   /* Create an endpoint.
      Must supply a 32 octet key/shared secret
   */
   MST_Endpoint(const uint8_t* sharedSecret):_buffer(NULL),_bufferSize(0)
   {
        memcpy(_sharedSecret,sharedSecret,32);
        _sessionKeySendingCreated = false;
        _sessionKeyReceivingCreated = false;     
   
        aes_key_setup(_sharedSecret,_aesSchedule,256);
        memset(_CounterOwn,0,16);
        memset(_CounterPartner,0,16);
   }

   ~MST_Endpoint()
   {
      memset(_buffer,0,_bufferSize);
      delete[] _buffer;
      //wipe cipher secrets
      memset(_sharedSecret,0,32);
      memset(_sessionKeyOwn,0,32);
      memset(_sessionKeyPartner,0,32);
      memset(_CounterOwn,0,16);
      memset(_CounterPartner,0,16);

      memset(_aesSchedule,0,60*sizeof(int));
      memset(_aesScheduleSessionKeyOwn,0,60*sizeof(int));
      memset(_aesScheduleSessionKeyPartner,0,60*sizeof(int));
   }


   bool createSenderSession(uint8_t* encryptedOwnKey32)
   {
        RandomSource rs;
        if( rs.getRandomNumber256(_sessionKeyOwn) )//Baustelle
        {
           aes_encrypt(_sessionKeyOwn,_sessionKeyOwnEncrypted,_aesSchedule,256);
           aes_key_setup(_sessionKeyOwn,_aesScheduleSessionKeyOwn,256);
           memcpy(encryptedOwnKey32,_sessionKeyOwnEncrypted,32); 
           _sessionKeySendingCreated = true;
           return false;
        }
        return true;
   }

   bool createSessionReceive(uint8_t* encryptedPartnerKey32)
   {
        aes_decrypt(encryptedPartnerKey32,_sessionKeyPartner,_aesSchedule,256);
        aes_key_setup(_sessionKeyPartner,_aesScheduleSessionKeyPartner,256);
        _sessionKeyReceivingCreated = true;
        return true;
   }
   

   /* Decrypt the SecuredMessage PDU received from the partner. 
      Returns bool on successful decryption, masking and validation (HMAC) of the decrypted PDU.
      The returned buffer is valid until the next method call of the class instance.
   */
   bool decryptSecureMessage(uint8_t* securedMessage, //SecuredMessage PDU
                             uint32_t lengthSM,       
                             uint8_t** plaintext,  //plaintext output, typically a reference to a pointer
                             uint32_t* lengthPlaintext) //plaintext length, typically a refrence to a uint32_t
   {
      if( !_sessionKeyReceivingCreated )   return false; 

      if( (lengthSM < (32*2)) || ((lengthSM & 0x1F) != 0 ) ) return false;

      if( !ensureBufferSize(lengthSM) )  return false; 

      for(uint32_t i=0; i < lengthSM; i+=16)
      {
         uint8_t aesZaehler[16];
         aes_encrypt(_CounterPartner,aesZaehler,_aesScheduleSessionKeyPartner,256);
         //XOR deciphering
         for(uint8_t j=0; j < 16; j++)
         {
            _buffer[i+j] ^= aesZaehler[j]; 
         }
         incrementMC(_CounterPartner);
      } 
      if( _mstHash.checkAndExtractPlaintext(_buffer,lengthSM,plaintext,lengthPlaintext) )
      {
         return true;
      }
      //Note: Reception of faulty PDUs means the destruction of the session, as the
      //      _CounterPartner is now probably out of sync. Higher software levels must
      //      handle this properly.
      return false;
   }   


   /*encrypt plaintext and generate the SecureMessage PDU. The resulting PDU 
     must then be sent to the communications  partner via any suitable 
     method of transport (e.g. TCP/IP, RS232, ATM, CAN,...)
   */
   bool encryptToSecureMessage(uint8_t* plaintext,
                               uint32_t length,
                               uint8_t** securedMessage,
                               uint32_t* lengthSM)
   {
       if( !_sessionKeySendingCreated) return false;
        
       if( _mstHash.createAuthPlaintext(plaintext,length,securedMessage,lengthSM) )
       {
	      for(uint32_t i=0; i < (*lengthSM); i+=16)
	      {
            //apply masking
            uint8_t aes_zaehler[16];
            aes_encrypt(_CounterOwn,aes_zaehler,_aesScheduleSessionKeyOwn,256);
            for(uint8_t j=0; j < 16; j++)
            {
               (*securedMessage)[i+j] ^= aes_zaehler[j];
            }
            incrementMC(_CounterOwn);
	      }
         return true; 
       }
       else return false;
   }

   
};


#endif



