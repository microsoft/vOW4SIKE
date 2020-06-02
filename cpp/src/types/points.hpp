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
        digit_t *words;       /* Word represendigit_tation of size NWORDS_STATE */
    };

    /*
    // virtual methods would be elegant (?) to have, but add 1 word per Point object
    // due to requiring a pointer to the virtual method table
    virtual bool operator==(const IPoint &other) const = 0;
    virtual bool operator!=(const IPoint &other) const = 0;
    virtual bool is_distinguished(uint64_t function_version) = 0;
    virtual uint64_t mem_index(uint64_t random_functions) = 0; // maybe could be a Memory method instead
    virtual void sample(IPRNG *prng) = 0;
    virtual void sample(uint64_t seed) = 0;
    virtual void from_point(const IPoint &s) = 0; // may want MyClass& operator= (const MyClass &);
    virtual void reset() = 0;
    */
};

class point_exception : public std::exception
{
    virtual const char *what() const throw()
    {
        return "Nwords_state for class Point not set";
    }
};

// NOTE: would be faster to swap pointers and keep track, maybe
template <class Point>
void SwapPoint(Point &r, Point &s);