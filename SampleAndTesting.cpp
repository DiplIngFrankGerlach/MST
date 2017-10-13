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



void testProc(uint32_t testSize)
{
   cout << "testProc size=" << testSize << endl;
   uint8_t* testData = new uint8_t[testSize];

   for(uint32_t i=0; i < testSize; i++)
   {
       testData[i] = uint8_t(i & 0xFF);
   }


   uint8_t* authPlaintext(NULL);
   uint32_t lengthAuth;
    
   MST_Hash mstHash;
   assert(mstHash.createAuthPlaintext( testData,testSize,&authPlaintext,&lengthAuth) );

   uint8_t* plaintextBack;  
   uint32_t lengthPT;

   //test:authPlaintext[6] ^= 1;

   assert( mstHash.checkAndExtractPlaintext(authPlaintext, lengthAuth,&plaintextBack,&lengthPT) );
   assert(testSize == lengthPT);
   assert(memcmp(testData,plaintextBack,testSize) == 0 );

   delete[] testData;
}

void encryptedCommunicationExample()
{
   uint8_t sharedKey[16] = {11,55,11,33,67,89,91,11,55,11,33,67,89,91,0,0};

   MST_Endpoint endpointA(sharedKey);
   MST_Endpoint endpointB(sharedKey);

   uint8_t mcA[16];
   endpointA.generateMaskCounterExchange(mcA);

   uint8_t mcB[16];
   endpointB.generateMaskCounterExchange(mcB);

   //exchange of masking counters
   endpointA.decryptMaskCounterExchange(mcB);
   endpointB.decryptMaskCounterExchange(mcA);
 
   
   //now we can securely encrypt with all assurances of the protocol
   const char* plaintext = "Schiller, Goethe, Von der Vogelweide";
   uint8_t* secureMessage;
   uint32_t lengthSM;
   uint32_t lp = strlen(plaintext)+1;
   assert( endpointA.encryptToSecureMessage((uint8_t*)plaintext,lp,&secureMessage,&lengthSM) );
   Util::dumpHex(secureMessage,lengthSM);

   uint8_t* plaintextDec;
   uint32_t lengthPT;
   assert( endpointB.decryptSecureMessage(secureMessage,lengthSM,&plaintextDec,&lengthPT) );

   cout << plaintextDec << endl;

   assert( endpointA.encryptToSecureMessage((uint8_t*)plaintext,lp,&secureMessage,&lengthSM) );
   Util::dumpHex(secureMessage,lengthSM);

   cout << plaintextDec << endl;
}

void encryptedCommunicationFailTest()
{
   uint8_t sharedKey[16] = {11,55,11,33,67,89,91,11,55,11,33,67,89,91,0,0};

   MST_Endpoint endpointA(sharedKey);
   MST_Endpoint endpointB(sharedKey);

   uint8_t mcA[16];
   endpointA.generateMaskCounterExchange(mcA);

   uint8_t mcB[16];
   endpointB.generateMaskCounterExchange(mcB);

   //exchange of masking counters
   endpointA.decryptMaskCounterExchange(mcB);
   endpointB.decryptMaskCounterExchange(mcA);
 
   
   //now we can securely encrypt with all assurances of the protocol
   const char* plaintext = "Schiller, Goethe, Von der Vogelweide";
   uint8_t* secureMessage;
   uint32_t lengthSM;
   uint32_t lp = strlen(plaintext)+1;
   assert( endpointA.encryptToSecureMessage((uint8_t*)plaintext,lp,&secureMessage,&lengthSM) );
   
   //flip a bit
   secureMessage[0] ^= 1;   

   uint8_t* plaintextDec;
   uint32_t lengthPT;
   assert( endpointB.decryptSecureMessage(secureMessage,lengthSM,&plaintextDec,&lengthPT) == false);
}

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

   testProc(1); 
   //testProc(0); 
   testProc(16); 
   testProc(15); 
   testProc(77); 
   testProc(32);
   testProc(256);
   testProc(2200);
   testProc(2048);
   //testProc(1000000);

   {
	   uint8_t ctr[]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	   uint8_t desiredResult[]={2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	   MST_Endpoint::incrementMC(ctr);
	   assert(memcmp(ctr,desiredResult,16)==0);

   }
   {   
	   uint8_t ctr[]={255,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	   uint8_t desiredResult[]={0,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	   MST_Endpoint::incrementMC(ctr);

	   assert(memcmp(ctr,desiredResult,16)==0);
   }
   {   
	   uint8_t ctr[]={255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
	   uint8_t desiredResult[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	   MST_Endpoint::incrementMC(ctr);

	   assert(memcmp(ctr,desiredResult,16)==0);
   }
   {   
	   uint8_t ctr[]={255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,255};
	   uint8_t desiredResult[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255};
	   MST_Endpoint::incrementMC(ctr);

	   assert(memcmp(ctr,desiredResult,16)==0);
   }
   encryptedCommunicationExample();
   encryptedCommunicationFailTest();
  
}
