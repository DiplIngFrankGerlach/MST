/*********************************************************************************
* Minimal Secure Transport Library
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/



/* Test Code for the Hash-MAC function and the AuthPlaintext PDU */

#include <assert.h>
#include "DaviesMeyerHash.h"
#include "Protocol.h"
#include "UnitTest.h"


int main()
{
   char plaintext[32];  

   strcpy(plaintext,"1123456789012345678901234567890");

   uint8_t hash[16];   
   DM_Hash::hash((uint8_t*) plaintext,32,hash);

   Util::dumpHex(hash,16); 

   char testInput[100];
   strcpy(testInput,"helloWorld");


   uint8_t* authPlaintext(NULL);
   uint32_t length(0);
   Util::dumpHex((uint8_t*)testInput,strlen(testInput));
   MST_Hash mstHash;
   if(mstHash.createAuthPlaintext( (uint8_t*)testInput,strlen(testInput)+1,&authPlaintext,&length) )
   {
      Util::dumpHex(authPlaintext,length);
   }
   else cout << "failure" << endl;

   uint8_t* plaintextBack;  
   uint32_t lengthPT;
   if( mstHash.checkAndExtractPlaintext(authPlaintext, length,&plaintextBack,&lengthPT) )
   {
      cout << ((char*)plaintext) << endl; 
   }
   else cout << "failure check" << endl;

   unitTestSuite(); 
  
}
