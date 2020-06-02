#pragma once 

#include "interface.hpp"
extern "C"
{
#include "../dependencies/aes/aes.h"
}

class AESPRNG : public IPRNG
{
protected:
    unsigned char aes_key_schedule[16 * 11] ALIGN_FOOTER(32);
    uint64_t count;
public:
    virtual ~AESPRNG() {};
    AESPRNG(unsigned long _seed);
    void seed(unsigned long _seed);
    void sample(unsigned char *buffer, unsigned long nbytes);
};