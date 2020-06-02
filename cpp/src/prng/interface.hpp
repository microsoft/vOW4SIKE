#pragma once

class IPRNG
{
public:
    virtual void seed(unsigned long _seed) = 0; // should be long enough for cryptanalysis
    virtual void sample(unsigned char *buffer, unsigned long nbytes) = 0;
};
