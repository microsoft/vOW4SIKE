#include <cstdio>
#include <omp.h>
#include "triples.hpp"

template <class Point, class Instance>
Trip<Point, Instance>::Trip(Instance *instance)
{
    initial_state = new Point(instance);
    current_state = new Point(instance);
};

template <class Point, class Instance>
Trip<Point, Instance>::~Trip()
{
    delete initial_state;
    delete current_state;
};

template <class Point, class Instance>
void Trip<Point, Instance>::sample(IPRNG *prng)
{
    current_state->sample(prng);
    current_steps = 0;
    initial_state->from_point(*current_state);
}

template <class Point, class Instance>
void Trip<Point, Instance>::from_trip(const Trip<Point, Instance> &s)
{
    current_steps = s.current_steps;
    current_state->from_point(*s.current_state);
    initial_state->from_point(*s.initial_state);
}

template <class Point, class Instance>
void Trip<Point, Instance>::from_trip(const Trip<Point, Instance> *s)
{
    current_steps = s->current_steps;
    current_state->from_point(*s->current_state);
    initial_state->from_point(*s->initial_state);
}

template <class Point, class Instance>
void Trip<Point, Instance>::reset()
{
    current_steps = 0;
    current_state->reset();
    initial_state->reset();
}

template <class Point, class Instance>
void Trip<Point, Instance>::print()
{
    initial_state->print();
    printf(" -(%" PRIu64 ")-> ", current_steps);
    current_state->print();
}

#include "../templating/triples.inc"