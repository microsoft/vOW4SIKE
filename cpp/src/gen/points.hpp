#pragma once

#include "../types/points.hpp"
#include "instance.hpp"
#include "../prng/interface.hpp"


class GenPoint : public IPoint
{
public:
    GenInstance *instance;
    void initialize(GenInstance *_instance);
    GenPoint(GenInstance *_instance);
    GenPoint(GenPoint *_point);
    ~GenPoint();
    bool operator==(const IPoint &other) const;
    bool operator!=(const IPoint &other) const;
    bool is_distinguished(uint64_t function_version);
    uint64_t mem_index(uint64_t random_functions);
    void sample(IPRNG *prng);
    void sample(uint64_t seed);
    void from_point(const IPoint &s); // may want MyClass& operator= (const MyClass &);
    void from_buf(const digit_t *s); // may want MyClass& operator= (const digit_t *);
    void to_buf(digit_t *r);
    void print();
    void reset();
};
