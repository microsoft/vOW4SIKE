#pragma once
#include <cstdint>
#include <exception>
#include "../config.h"
#include "../types/triples.hpp"

template <class Point, class Instance>
class IMemory
{
    public:
        virtual Trip<Point, Instance> *operator[](uint64_t i) = 0;
        virtual bool read(Trip<Point, Instance> **t, uint64_t address) = 0;
        virtual void write(Trip<Point, Instance> *t, uint64_t address) = 0;
        virtual void reset() = 0;
};

class memory_exception : public std::exception
{
  virtual const char* what() const throw()
  {
    return "Could not initialise memory";
  }
};