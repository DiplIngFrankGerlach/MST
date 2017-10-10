#include <string.h>
#include "DummyCipher.h"
#include "Util.h"

uint32_t bitDiff(uint8_t* octets32,uint8_t* octets32B)
{
   uint32_t diffCount(0);
   for(uint8_t i=0; i < 32; i++)
   {
     uint8_t octetA=octets32[i]; 
     uint8_t octetB=octets32B[i]; 
     for(uint8_t j=0; j < 8; j++)
     {
         
         if( ((octetA >>j)&1) != ((octetB>>j)&1) ) 
         {
           diffCount++;
         }
     } 
   }
   return diffCount;
}

void cryptoTest(DummyCipher* dc)
{
    uint8_t resultBeforeBuf[32];
    memset(resultBeforeBuf,0,32);
    for(uint16_t i=0; i < 200; i++)
    {
       uint8_t cryptBuffer[]=
          {0,1,1,3,4,5,6,7,8,9,0xA,0xB,0xC,0xD,0xE,0xF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

       cryptBuffer[i/8] ^= (1 << (i%8));
       dc->encrypt(cryptBuffer);
       cout << "diff: " << bitDiff(cryptBuffer,resultBeforeBuf) << endl; 
       memcpy(resultBeforeBuf,cryptBuffer,32);
    }
}


int main(int argc,char** argv)
{
   int runden;
   sscanf(argv[1],"%i",&runden);
   const char* key = (const char*)"derSchnelleAffeKriegtMehrBananen1111";
   DummyCipher dc(runden);
   dc.setKey(key,strlen(key));

   uint8_t cryptBuffer[]=
     {0,1,1,3,4,5,6,7,8,9,0xA,0xB,0xC,0xD,0xE,0xF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

   Util::dumpHex((uint8_t*) cryptBuffer,32);


   dc.encrypt(cryptBuffer);

   
   Util::dumpHex((uint8_t*) cryptBuffer,32);

   {
       uint8_t cryptBuffer2[]=
         {0,1,2,3,4,5,6,7,8,9,0xA,0xB,0xC,0xD,0xE,0xF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
       dc.encrypt(cryptBuffer2);
       Util::dumpHex((uint8_t*) cryptBuffer2,32);
   }
   {
       uint8_t cryptBuffer2[]=
         {0,1,2,3,4,5,6,7,8,9,0xA,0xB,0xC,0xD,0xE,0xF,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0};
       dc.encrypt(cryptBuffer2);
       Util::dumpHex((uint8_t*) cryptBuffer2,32);
   }
   {
       uint8_t cryptBuffer2[32];
       memset(cryptBuffer2,0,32);
       dc.encrypt(cryptBuffer2);
       Util::dumpHex((uint8_t*) cryptBuffer2,32);
   }
   {
       uint8_t cryptBuffer2[32];
       memset(cryptBuffer2,0,32);
       cryptBuffer2[0]=8;
       dc.encrypt(cryptBuffer2);
       Util::dumpHex((uint8_t*) cryptBuffer2,32);
   }
   {
       uint8_t cryptBuffer2[32];
       memset(cryptBuffer2,0,32);
       cryptBuffer2[30]=8;
       dc.encrypt(cryptBuffer2);
       Util::dumpHex((uint8_t*) cryptBuffer2,32);
   } 
   cryptoTest(&dc);

}
