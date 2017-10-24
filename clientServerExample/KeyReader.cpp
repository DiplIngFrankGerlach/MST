/*********************************************************************************
* Class for reading pre-shared keys from file. 
*
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "KeyReader.h"


KeyReader::KeyReader():_fileHandle(-1),_readBuffer(NULL),_bufferValid(0),_readPtr(0)
{
   _readBufferSize = 2000;
   _readBuffer = new char[_readBufferSize];
   
}


bool KeyReader::fillBuffer()
{
  if( _fileHandle > 0)
  {
      int result = read(_fileHandle,_readBuffer,_readBufferSize);
      if( result > 0)
      {
          _bufferValid = result;
          _readPtr = 0;
          return true;
      }
  }
  return false;
}



bool KeyReader::getChar()
{
   if( _bufferValid == _readPtr )
   {
      if( !fillBuffer() )
      {
         return false;
      }
   }
   _currentChar = _readBuffer[_readPtr++];
   return true;
}

static bool isNumber(char c)
{
   switch(c)
   {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':return true;
      default: return false;
   }
}

static bool isHexNumber(char c,uint8_t &value)
{
   switch(c)
   {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':value = (c-'0');
               return true;
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
               value = (c-'A'+10);
               return true;
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
               value = (c-'a'+10);
      return true;
      default: return false;
   }
}


bool KeyReader::readNumber(uint32_t* number)
{
   *number = 0;
   uint8_t i=0;
   while( isNumber(_currentChar) && (i < 10))
   {
      (*number) += (_currentChar - '0');
      i++;
      if(!getChar() )
      {
        break;
      }
      else
      {
         if( (i < 9) && isNumber(_currentChar) )
         {
            (*number) *= 10;
         }
      }
      
   }
   return (i > 0);
}

bool KeyReader::readPSKey(uint8_t* key)
{
   uint8_t i=0;
   
   while( i < 16 )
   {
      uint8_t valueTwo(0);
      uint8_t value;
      bool success = false;
      if( isHexNumber(_currentChar,value) )
      {
         valueTwo = value;
         valueTwo <<= 4;
         if( getChar() )
         {
            if( isHexNumber(_currentChar,value) )
            {
               valueTwo += value;
               if( i < 15 )
               {     
                  if( getChar() )
                  {
                     success = true;
                  }
               }
               else
               {
                 success = true;
               }
            }
         }
      }
      
      if( !success)
      {
         return false;
      }
      key[i++] = valueTwo;
      
      if( i < 16 )
      {
         success = false;
         if( _currentChar == '-' )
         {
            if( getChar() )
            {
                success = true;
            }
         }
         if( !success)
         {
            return false;
         }
      }
      
   }
   return true;
}

bool KeyReader::readPresharedKeys()
{
   _fileHandle = open("./presharedKeys.csv",O_RDONLY);
   if( _fileHandle < 1)
   {
      return false;
   }

   if( !getChar() )
   {
     return true;//an empty list of passwords is also OK
   }

   bool moreLines;
   do
   {
      uint32_t partnerNumber(0);
      uint8_t presharedKey[16];
      bool success = false;
      if( readNumber(&partnerNumber) )
      {
           if( (_currentChar == ':') && getChar() )
           {
               if( readPSKey(presharedKey) )
               {
                  success = true;
               }
           }
      }
      if( !success )
      {
         return false;
      }
      _presharedKeys[partnerNumber] = PresharedKey(presharedKey);

      moreLines = false;
      if( getChar() )
      {
         if(_currentChar == '\n')
         {
           if( getChar() )
           {
              moreLines = true;
           }
         }
      }
   }
   while( moreLines );

   
   return true;
}

bool KeyReader::getKeyForPartnerID(uint32_t partnerID, uint8_t** key)
{ 
   if( _presharedKeys.find(partnerID) != _presharedKeys.end() )
   {
      _presharedKeys[partnerID].get(key);
      return true;
   }
   return false;
}

KeyReader::~KeyReader()
{
   delete[] _readBuffer;
   _readBuffer = NULL;
   close(_fileHandle);
   _fileHandle = 0;   
}


KeyReader __keyReader;




