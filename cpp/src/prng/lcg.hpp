#pragma once

#include "interface.hpp"

class LCG : public IPRNG
{
private:
    unsigned long A;
    unsigned long B;
    unsigned long rand_max;
    unsigned long sampled;
public:
    LCG(unsigned long _seed);
    virtual ~LCG() {};
    void seed(unsigned long _seed);
    void sample(unsigned char *buffer, unsigned long nbytes);
};