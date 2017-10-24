#include <assert.h>
#include "KeyReader.h"


int main()
{
    KeyReader kr;
    assert(kr.readPresharedKeys());

    uint8_t* key;
    assert(kr.getKeyForPartnerID(104,&key) && (key != NULL));
    assert( key[0] = 0xb4);
}


