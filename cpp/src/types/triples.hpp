#pragma once
#include <cinttypes>
#include <exception>
#include <string>
#include "../config.h"
#include "instance.hpp"
#include "points.hpp"
#include "../prng/interface.hpp"

template <class Point, class Instance>
class Trip
{
public:
    Point *current_state; // todo: rename to current/initial_point?
    uint64_t current_steps = 0;
    Point *initial_state;
    Trip(Instance *instance);
    ~Trip();
    void sample(IPRNG *prng);
    void from_trip(const Trip<Point, Instance> &s);
    void from_trip(const Trip<Point, Instance> *s);
    void reset();
    void print();
};