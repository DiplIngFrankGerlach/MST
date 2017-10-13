/*********************************************************************************
* Minimal Secure Transport Library
*
* Free for non-Commercial Use. Commercial Use requires a license from the author.
*
* Copyright (C) 2017 Frank Gerlach, frankgerlach.tai@gmx.de
*
**********************************************************************************/
#ifndef RANDOMSOURCE_HEADER
#define RANDOMSOURCE_HEADER

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* a source of cryptographically strong pseudo-random numbers, 
   based on an initialization vector of 128bit Randomness supplied by
   the user or the operating system (e.g. /dev/random on Linux)

   Other sources of randomness, including Windows and stand-alone Randomness
   Sources can be implemented on request. Please inquire at frankgerlach.tai@gmx.de
   for that.
*/
class RandomSource
{
public:
    bool getRandomNumber128(uint8_t* destination)
    {
        int randomFH = open("/dev/urandom", O_RDONLY);
	if (randomFH >= 0)
	{
	    ssize_t result = read(randomFH, destination, 16);
            close(randomFH);
	    if (result == 16)
	    {
		return true;
	    }
	}
        return false;
    }
};

#endif
