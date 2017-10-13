/*********************************************************************************
* Minimal Secure Transport Library
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/

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


void encryptedCommunicationExampleComplex()
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

   for(uint32_t i=0; i < 10000; i++)
   {
	   string plaintext = "Schiller, Goethe, Von der Vogelweide";

           uint8_t n= rand() & 0xF;
           for(uint8_t j=0; j < n; j++)
           {
              plaintext += " three more words";
           }
	   uint8_t* secureMessage;
	   uint32_t lengthSM;
	   assert( endpointA.encryptToSecureMessage((uint8_t*)plaintext.c_str(),
		                                    plaintext.length()+1,
		                                    &secureMessage,
		                                    &lengthSM) );
	   
	   uint8_t* plaintextDec;
	   uint32_t lengthPT;
	   assert( endpointB.decryptSecureMessage(secureMessage,lengthSM,&plaintextDec,&lengthPT) );

	   assert( plaintext.compare( (const char*)plaintextDec) == 0 );


           if( (rand() & 0xF) < 5 )
           {
		   string plaintext2 = "Buy 100 shares of Apple and 200 shares of Deutsche Bank";

		   n= rand() & 0xF;
		   for(uint8_t j=0; j < n; j++)
		   {
		      plaintext2 += " three more words";
		   }
		   assert( endpointB.encryptToSecureMessage((uint8_t*)plaintext2.c_str(),
				                            plaintext2.length()+1,
				                            &secureMessage,
				                            &lengthSM) );
	   
		   assert( endpointA.decryptSecureMessage(secureMessage,lengthSM,&plaintextDec,&lengthPT) );

		   assert( plaintext2.compare((const char*)plaintextDec) == 0 );
           }
   }
   
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

void replayAttackFailTest()
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
   const char* plaintext = "transfer $100 from Account No 19112119 to 1882122";
   uint8_t* secureMessage;
   uint32_t lengthSM;
   uint32_t lp = strlen(plaintext)+1;
   assert( endpointA.encryptToSecureMessage((uint8_t*)plaintext,lp,&secureMessage,&lengthSM) );

   uint8_t* replayMessage = new uint8_t[lengthSM];
   memcpy(replayMessage,secureMessage,lengthSM);
   
   uint8_t* plaintextDec;
   uint32_t lengthPT;
   assert( endpointB.decryptSecureMessage(secureMessage,lengthSM,&plaintextDec,&lengthPT));

   //now comes the replay attack
   assert( endpointB.decryptSecureMessage(replayMessage,lengthSM,&plaintextDec,&lengthPT) == false);

   delete[] replayMessage;
}

void duplicateObfuscationTest()
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
   const char* plaintext = "key up";
   uint8_t* secureMessage;
   uint32_t lengthSM;
   uint32_t lp = strlen(plaintext)+1;
   assert( endpointA.encryptToSecureMessage((uint8_t*)plaintext,lp,&secureMessage,&lengthSM) );

   uint32_t lengthSMSaved = lengthSM;
   uint8_t* secureMessageSaved = new uint8_t[lengthSM];
   memcpy(secureMessageSaved,secureMessage,lengthSM);
   
   assert( endpointA.encryptToSecureMessage((uint8_t*)plaintext,lp,&secureMessage,&lengthSM) );

   assert( (lengthSMSaved == lengthSM) && ( memcmp(secureMessageSaved,secureMessage,lengthSM) != 0 ) );

   cout << "the following two hex strings must look like uncorrelated noise strings:" << endl;
   Util::dumpHex(secureMessageSaved,lengthSM);
   Util::dumpHex(secureMessage,lengthSM);
   
   delete[] secureMessageSaved;
}

/* Unit Tests for the MST library */
void unitTestSuite()
{
   testProc(1); 
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

   replayAttackFailTest();
   duplicateObfuscationTest();
   
   encryptedCommunicationExampleComplex();
}

