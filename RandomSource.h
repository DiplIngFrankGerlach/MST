#ifndef RANDOMSOURCE_HEADER
#define RANDOMSOURCE_HEADER

#include <stdlib.h>

/* a source of cryptographically strong pseudo-random numbers, 
   based on an initialization vector of 128bit Randomness supplied by
   the user or the operating system (e.g. /dev/random on Linux)

   WARNING: ONLY A MOCKUP AT THE MOMENT !
*/
class RandomSource
{
public:
    void getRandomNumber128(uint8_t* destination)
    {
        for(uint8_t i=0; i < 16; i++)
        {
             destination[i] = rand();
        }
    }
};

#endif
