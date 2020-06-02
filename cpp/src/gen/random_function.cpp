#include <cstdio>
#include <omp.h>
#include "random_function.hpp"
#include "../settings.h"
#include "../prng/xof.h"
#include "../utils/fix_overflow.h"

template <class Point>
GenRandomFunction<Point>::GenRandomFunction(GenInstance *instance)
{
    image = new Point(instance);
    preimages[0] = new Point(instance);
    preimages[1] = new Point(instance);
    function_version = instance->initial_function_version;
    // the function version should be used together with a salted stateless XOF to seed the function
    // this way independently held random functions with th esame version and basic XOF should
    // have the same golden collision. To avoid insta-wins, we offset the salt for seeding from the
    // salt used for stepping
    seed(function_version);
}

template <class Point>
void GenRandomFunction<Point>::seed(uint64_t _seed)
{
    image->sample(_seed);
    sample_preimages(_seed);
}

template <class Point>
void GenRandomFunction<Point>::sample_preimages(uint64_t _seed)
{
    uint64_t offset = 1;
    do
    {
        preimages[0]->sample(_seed + offset++);
    } while (*(preimages[0]) == *image);

    do
    {
        preimages[1]->sample(_seed + offset++);
    } while (*(preimages[1]) == *(preimages[0]) || *(preimages[1]) == *image);
}

/**
 * @brief generate a new random function to walk on
 *
 * @param S
 * @param private_state
 */
template <class Point>
void GenRandomFunction<Point>::update()
{
    function_version++;
}

template <class Point>
void GenRandomFunction<Point>::eval(Point &out, Point &in)
{

    if (in == *preimages[0] || in == *preimages[1])
    {
        out.from_point(*image);
    }
    else
    {
        XOF(out.bytes, in.bytes, (unsigned long)out.instance->NBYTES_STATE, (unsigned long)in.instance->NBYTES_STATE, (unsigned long)function_version);
        fix_overflow(out.bytes, out.instance->NBYTES_STATE, out.instance->NBITS_OVERFLOW);
        if (out == *image)
        {
            out.words[0]++; // Force collision to be golden by avoiding natural preimages
                            // NOTE: does not require correctly handling carry bit
        }

        fix_overflow(out.bytes, out.instance->NBYTES_STATE, out.instance->NBITS_OVERFLOW);
    }
}

template <class Point>
void GenRandomFunction<Point>::eval(Point &x)
{
    // performs update in place
    eval(x, x);
}

template <class Point>
GenRandomFunction<Point>::~GenRandomFunction()
{
    delete image;
    delete preimages[0];
    delete preimages[1];
}

// now instantiate templates
#include "points.hpp"
template class GenRandomFunction<GenPoint>;