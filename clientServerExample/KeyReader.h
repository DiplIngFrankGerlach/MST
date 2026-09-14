/*********************************************************************************
* Class for reading pre-shared keys from file. 
*
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017,2026 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/


#ifndef KEY_READER_HEADER
#define KEY_READER_HEADER

#include <unordered_map>
#include <memory.h>
#include <stdint.h>
#include "Util.h"
#include "Protocol.h"

using namespace std;

/* a class encapsulating a 256 bit symmetric key, needed for unordered_map */
class PresharedKey
{
   uint8_t _key[AES_KEY_SIZE];
public:
   PresharedKey()
   {
     memset(_key,0,AES_KEY_SIZE);
   }
   PresharedKey(uint8_t* key)
   {
      memcpy(_key,key,AES_KEY_SIZE);
   }

   PresharedKey(const PresharedKey& other)
   {
      memcpy(_key,other._key,AES_KEY_SIZE);
   }

   void operator=(const PresharedKey& other)
   {
      memcpy(_key,other._key,AES_KEY_SIZE);
   }

   void get(uint8_t** destination)
   {
     *destination = _key;
   }

   void print() const
   {
       Util::dumpHex(_key,AES_KEY_SIZE); 
   }
};

/* This class reads the Pre-Shared Keys for each Partner ID */
class KeyReader
{
   unordered_map<uint32_t,PresharedKey> _presharedKeys;

   int   _fileHandle;
   char* _readBuffer;
   uint32_t _readBufferSize;
   uint32_t _bufferValid;
   uint32_t _readPtr;

   char _currentChar;

   bool fillBuffer();

   bool getChar();
   bool readNumber(uint32_t* number);
   

   
public:
   KeyReader();

   ~KeyReader();

   bool readPSKey(uint8_t* key);

   bool loadOneKeyToBuffer(const char* asciiRepresentation);

   bool readPresharedKeys();

   bool getKeyForPartnerID(uint32_t partnerID, uint8_t** key);
};

extern KeyReader __keyReader;

#endif
