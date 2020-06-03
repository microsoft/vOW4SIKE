#include "../vow.hpp"
#include "../gen/points.hpp"
#include "../gen/random_function.hpp"
#include "../gen/instance.hpp"
#include "../memory/ram.hpp"
#include "../prng/aes.hpp"
#include "../prng/lcg.hpp"
typedef GenInstance Instance;
typedef GenPoint Point;
typedef LocalMemory<Point, Instance> Memory;
typedef AESPRNG PRNG;
typedef GenRandomFunction<Point> RandomFunction;