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

#define AES_WORDSIZE 16

#define AES_KEY_SIZE 32
#define AES_KEY_SIZE_HALF (AES_KEY_SIZE / 2)
#define AES_KEY_SIZE_BITS (AES_KEY_SIZE*8)
#define COUNTER_SIZE 16


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
          cout << "Error 1 in createAuthPlaintext" << endl;
          return false;
        }

        //cout << "length:" << length << endl;

        uint32_t oktetsMitLaengenAnzeiger = length + 4;

        uint32_t laengePadding = HashWordSize - (oktetsMitLaengenAnzeiger % HashWordSize);

        uint32_t neededSize = oktetsMitLaengenAnzeiger + laengePadding + HashWordSize;

        //cout << "neededSize: " << neededSize << endl;
 
        if( _outputBufferSize < neededSize )
        {
           if( _outputBuffer != NULL )
           {
              memset(_outputBuffer,0,_outputBufferSize);
              delete[] _outputBuffer;
           }
           
           _outputBufferSize = neededSize;
           _outputBuffer = new uint8_t[_outputBufferSize];
           if( _outputBuffer == NULL )
           {
              return false;
           }
        }

        //cout << "_outputBuffer:" << ((void*)_outputBuffer) << endl;

        //write message length
        Util::int2Octets(length,_outputBuffer);
        //write message
        memcpy(_outputBuffer+4,sourcePlaintext,length);

        //zero out the rest of the last 32 Octet word(the padding)
        uint32_t pos;
        for( pos = length+4; pos < (oktetsMitLaengenAnzeiger + laengePadding); pos++)
        {
           //cout << "ausnullen Padding " << pos << endl;
           _outputBuffer[pos]=0;
        }

        
        

        //cout << "pos:" << pos << endl;
       
         
        if( DM_Hash::hash(_outputBuffer,pos,_outputBuffer+pos) == false)
        {
           return false;
        }
        *outputBuffer = _outputBuffer;
        *outputSize = neededSize;
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
      if( (length < (2*HashWordSize)) || ((length % HashWordSize) != 0) )
      {
        cout << "illegal PDU size" << endl;
        return false;
      }
      uint8_t computedHash[HashWordSize];

      //cout << "checkAndExtractPlaintext length-HashWordSize=" << (length-HashWordSize) << endl;

      if(DM_Hash::hash(authPlaintext,length - HashWordSize,computedHash) )
      {
         uint32_t authenticatedLength = length - HashWordSize; 
         if( timing_insensitive_memcmp(computedHash,authPlaintext+authenticatedLength,HashWordSize)  )
         {
            Util::octets2Int(authPlaintext,lengthPlaintext);
            if( (*lengthPlaintext) < (length - HashWordSize) )
            {
               *plaintext = authPlaintext+4;
               return true;
            }            
            else
            {
               plaintext = NULL;
               lengthPlaintext = 0;
               return false;
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
   uint8_t _sharedSecret[AES_KEY_SIZE];      //the secret key shared by the communication partners
   uint8_t _sessionKeyOwn[AES_KEY_SIZE];
   uint8_t _sessionKeyOwnEncrypted[AES_KEY_SIZE];
   uint8_t _sessionKeyPartner[AES_KEY_SIZE];
   uint8_t _CounterOwn[COUNTER_SIZE];    //the MaskingCounter of this endpoint
   uint8_t _CounterPartner[COUNTER_SIZE];//the MaskingCounter of the other endpoint
  


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
         if(_buffer != NULL)
         {
            memset(_buffer,0,_bufferSize);
            delete[] _buffer;
         }
         _bufferSize = sz;
         //cout << "allocating _buffer size " << _bufferSize << endl;
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
       while(carry && (i < COUNTER_SIZE));
   }

   /* Create an endpoint.
      Must supply a 32 octet key/shared secret
   */
   MST_Endpoint(const uint8_t* sharedSecret):_buffer(NULL),_bufferSize(0)
   {
        memcpy(_sharedSecret,sharedSecret,AES_KEY_SIZE);
        _sessionKeySendingCreated = false;
        _sessionKeyReceivingCreated = false;     
   
        aes_key_setup(_sharedSecret,_aesSchedule,AES_KEY_SIZE_BITS);
        memset(_CounterOwn,0,COUNTER_SIZE);
        memset(_CounterPartner,0,COUNTER_SIZE);
   }

   ~MST_Endpoint()
   {
      memset(_buffer,0,_bufferSize);
      delete[] _buffer;
      //wipe cipher secrets
      memset(_sharedSecret,0,AES_KEY_SIZE);
      memset(_sessionKeyOwn,0,AES_KEY_SIZE);
      memset(_sessionKeyPartner,0,AES_KEY_SIZE);
      memset(_CounterOwn,0,COUNTER_SIZE);
      memset(_CounterPartner,0,COUNTER_SIZE);

      memset(_aesSchedule,0,60*sizeof(int));
      memset(_aesScheduleSessionKeyOwn,0,60*sizeof(int));
      memset(_aesScheduleSessionKeyPartner,0,60*sizeof(int));
   }


   bool createSenderSession(uint8_t* encryptedOwnKey32)
   {
        RandomSource rs;
        if( rs.getRandomNumber256(_sessionKeyOwn) )
        {
           aes_encrypt(_sessionKeyOwn,_sessionKeyOwnEncrypted,_aesSchedule,AES_KEY_SIZE_BITS);
           aes_encrypt(_sessionKeyOwn + (AES_KEY_SIZE/2),_sessionKeyOwnEncrypted + (AES_KEY_SIZE/2),_aesSchedule,AES_KEY_SIZE_BITS);
           aes_key_setup(_sessionKeyOwn,_aesScheduleSessionKeyOwn,AES_KEY_SIZE_BITS);
           memcpy(encryptedOwnKey32,_sessionKeyOwnEncrypted,AES_KEY_SIZE); 
           _sessionKeySendingCreated = true;
           return false;
        }
        return true;
   }

   bool createSessionReceive(uint8_t* encryptedPartnerKey32)
   {
        aes_decrypt(encryptedPartnerKey32,_sessionKeyPartner,_aesSchedule,AES_KEY_SIZE_BITS);
        aes_decrypt(encryptedPartnerKey32+AES_KEY_SIZE_HALF,_sessionKeyPartner+AES_KEY_SIZE_HALF,_aesSchedule,AES_KEY_SIZE_BITS);
        aes_key_setup(_sessionKeyPartner,_aesScheduleSessionKeyPartner,AES_KEY_SIZE_BITS);
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

      if( (lengthSM < (HashWordSize*2)) || ((lengthSM % HashWordSize) != 0 ) ) return false;
 
      //cout << "lengthSM:" << lengthSM << endl;

      if( !ensureBufferSize(lengthSM + 1) )  return false; 

      //cout << "lengthSM:" << lengthSM  << endl;

      for(uint32_t i=0; i < lengthSM; i += AES_WORDSIZE)
      {
         //cout << "decryptSecureMessage " << i << endl;
         uint8_t aesZaehler[AES_WORDSIZE];
         aes_encrypt(_CounterPartner,aesZaehler,_aesScheduleSessionKeyPartner,HashWordSizeBits);
         //XOR deciphering
         for(uint8_t j=0; j < AES_WORDSIZE; j++)
         {
            uint32_t buffer_pos = i+j;
            //cout << "buffer_pos:" << buffer_pos << endl; 
            _buffer[buffer_pos] = securedMessage[buffer_pos] ^ aesZaehler[j]; 
         }
         incrementMC(_CounterPartner);
      } 
      //_buffer[lengthSM] = 0;//unscharf !
      //cout << "buffer:" << _buffer << endl;

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
       if( !_sessionKeySendingCreated)
       {
           cout << "Session key has not been created !" << endl;
           return false;
       }
        
       if( _mstHash.createAuthPlaintext(plaintext,length,securedMessage,lengthSM) )
       {
	      for(uint32_t i=0; i < (*lengthSM); i += AES_WORDSIZE)
	      {
            //apply masking
            uint8_t aes_zaehler[AES_WORDSIZE];
            aes_encrypt(_CounterOwn,aes_zaehler,_aesScheduleSessionKeyOwn,HashWordSize);
            for(uint8_t j=0; j < AES_WORDSIZE; j++)
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



