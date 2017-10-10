#include <iostream>
#include "aes.h"

using namespace std;

/*

void aes_key_setup(const BYTE key[],          // The key, must be 128, 192, or 256 bits^M
                   WORD w[],                  // Output key schedule to be used later^M
                   int keysize);              // Bit length of the key, 128, 192, or 256^M
^M
void aes_encrypt(const BYTE in[],             // 16 bytes of plaintext^M
                 BYTE out[],                  // 16 bytes of ciphertext^M
                 const WORD key[],            // From the key setup^M
                 int keysize);                // Bit length of the key, 128, 192, or 256^M
^M
void aes_decrypt(const BYTE in[],             // 16 bytes of ciphertext^M
                 BYTE out[],                  // 16 bytes of plaintext^M
                 const WORD key[],            // From the key setup^M
                 int keysize);                // Bit length of the key, 128, 192, or 256^M

*/


int main()
{
    uint8_t schluessel[16];
    for(uint8_t i=0; i < 16; i++)
    {
        schluessel[i]=i;
    } 
    schluessel[0]=1;
    uint32_t schedule[60];
    aes_key_setup(schluessel,schedule,128);

    
    uint8_t klartext[16];
    for(uint8_t i=0; i < 16; i++)
    {
        klartext[i]=i;
    } 
    uint8_t chiffreText[16];

    aes_encrypt(klartext,chiffreText,schedule,128);


    for(uint8_t i=0; i < 16; i++)
    {
        cout << uint32_t(chiffreText[i]) << " ";
    } 
    cout << endl;
}
