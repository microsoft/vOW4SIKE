#pragma once
#include <cinttypes>
#include <exception>
#include "../config.h"
#include "../prng/interface.hpp"
#include "instance.hpp"

class IPoint
{
public:
    union {
        unsigned char *bytes; /* Byte representation of size NBYTES_STATE */
        digit_t *words;       /* Word representation of size NWORDS_STATE */
    };
};

class point_exception : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Nwords_state for class Point not set";
    }
};

template <class Point>
void SwapPoint(Point &r, Point &s);