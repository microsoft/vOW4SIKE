#include "../gen/points.hpp"
#include "../gen/random_function.hpp"
#include "../gen/instance.hpp"
#include "../memory/ram.hpp"
#include "../prng/aes.hpp"
#include "../prng/lcg.hpp"
template class vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, AESPRNG, GenInstance>;
// template class vOW<GenPoint, LocalMemory<GenPoint, GenInstance>, GenRandomFunction<GenPoint>, LCG, GenInstance>;