#ifndef UTIL_HEADER
#define UTIL_HEADER

#include <stdint.h>
#include <iostream>

using namespace std;

class Util
{
  public:
  static void printNibble(uint8_t value)
  {
      if(value < 10)
      {
        cout << char('0'+value);
      }
      else
      {
        cout << char('A'-10+value);
      }
  }
  static void dumpHex(uint8_t* buffer,uint32_t length)
  {
     for(uint32_t i=0; i < length; i++)
     {
         uint8_t octet = buffer[i];

         uint8_t nibbleUpper = octet >> 4;
         uint8_t nibbleLower = octet & 0xF;
         printNibble(nibbleUpper);
         printNibble(nibbleLower);
     }
     cout << endl;
  } 

  static void int2Octets(uint32_t v,uint8_t* destination)
  {
     for(uint8_t i=0; i < 4; i++)
     {
         destination[i] = v & 0xFF;
         v >>=8;
     }
  }

  static void octets2Int(uint8_t* source,uint32_t* destination)
  {
     *destination = 0;
     for(int8_t i=3; i >= 0; i--)
     {
       (*destination) += source[i];
       if( i > 0 )
       {
         (*destination) <<= 8;
       }
     }
  }
};
#endif
