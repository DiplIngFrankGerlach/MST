#include <assert.h>
#include "Protocol.h"

/* This is a simple example how to use the MST library - all in a single
   program. In reality, there would be two programs (e.g. client and server)
   with one MST_Endpoint object each.
   Messages would be exchanged via TCP, RS232 or the like.
*/


int main()
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

}
