#pragma once

#include "interface.hpp"
#include "../types/instance.hpp"
#include "../types/triples.hpp"

template <class Point, class Instance>
class LocalMemory : public IMemory<Point, Instance>
{
    protected:
        uint64_t max_entries;
        Trip<Point, Instance> **memory;
    public:
        LocalMemory(uint64_t _max_entries, Instance *instance);
        virtual ~LocalMemory();
        Trip<Point, Instance> *operator[](uint64_t i);
        bool read(Trip<Point, Instance> **t, uint64_t address);
        void write(Trip<Point, Instance> *t, uint64_t address);
        void reset();
};
