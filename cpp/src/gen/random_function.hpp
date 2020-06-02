#pragma once
#include <cstdint>
#include "instance.hpp"
#include "../types/random_function.hpp"
#include "../types/points.hpp"
#include "../prng/interface.hpp"

template <class Point>
class GenRandomFunction : public IRandomFunction<Point>
{
protected:
    void sample_preimages(uint64_t _seed);
public:
    Point *preimages[2];

    uint64_t function_version;
    Point *image;
    GenRandomFunction(GenInstance *instance);
    virtual ~GenRandomFunction();
    void seed(uint64_t _seed);
    void update();
    void eval(Point &out, Point &in);
    void eval(Point &x);
};
