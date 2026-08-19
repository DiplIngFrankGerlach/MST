
#include <stdint.h>

#include "GeneralUtil.h"

bool timing_insensitive_memcmp(uint8_t* feld1, uint8_t* feld2, uint32_t anzahl)
{
    uint32_t anzahl_gleich;
    uint32_t anzahl_ungleich;
    for(uint32_t i=0; i < anzahl; i++)
    {
        if(feld1[i] == feld2[i]) anzahl_gleich++;
        if(feld1[i] != feld2[i]) anzahl_ungleich++;
    }
    if(anzahl_gleich != anzahl) return false;
    if(anzahl_ungleich != 0) return false;

    return true;
}

