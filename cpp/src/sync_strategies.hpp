#pragma once
#include <cstdint>
#include <cstring>
#include "types/resync_state.hpp"
#include "vow.hpp"
#include "types/state.hpp"

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void print_all_threads(const vOW<Point, Memory, RandomFunction, PRNG, Instance> &S, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state, unsigned char *buffer);

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool windowed_resync_should_resync(const vOW<Point, Memory, RandomFunction, PRNG, Instance> *vow, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);
template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void windowed_resync_do_resync(const vOW<Point, Memory, RandomFunction, PRNG, Instance> *vow, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);
template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void windowed_resync(const vOW<Point, Memory, RandomFunction, PRNG, Instance> *vow, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);

template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool stakhanovist_resync_should_resync(vOW<Point, Memory, RandomFunction, PRNG, Instance> *S, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);
template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void stakhanovist_resync_do_resync(vOW<Point, Memory, RandomFunction, PRNG, Instance> *S, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);

void nobiggie_sync_small_sleep(int s);
template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
bool nobiggie_resync_should_resync(vOW<Point, Memory, RandomFunction, PRNG, Instance> *S, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);
template <class Point, class Memory, class RandomFunction, class PRNG, class Instance>
void nobiggie_resync_do_resync(vOW<Point, Memory, RandomFunction, PRNG, Instance> *S, private_state_t<Point, PRNG, RandomFunction, Instance> &private_state);
