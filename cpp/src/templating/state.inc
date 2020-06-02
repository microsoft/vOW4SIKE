#include "../gen/points.hpp"
#include "../prng/aes.hpp"
#include "../prng/lcg.hpp"
#include "../gen/random_function.hpp"
#include "../gen/instance.hpp"
template class private_state_t<GenPoint, AESPRNG, GenRandomFunction<GenPoint>, GenInstance>;
template class private_state_t<GenPoint, LCG, GenRandomFunction<GenPoint>, GenInstance>;
