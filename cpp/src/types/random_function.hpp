#pragma once
#include "../prng/interface.hpp"

template <class Point>
class IRandomFunction
{
public:
    uint64_t function_version;
    Point *image;
    virtual void seed(uint64_t seed) = 0;
    virtual void update() = 0;
    virtual void eval(Point &out, Point &in) = 0;
    virtual void eval(Point &x) = 0;
};
