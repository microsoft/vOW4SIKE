#pragma once
#include <cstdint>
struct statistics_t
{
    double calendar_time;
    double total_time;
    double random_functions;
    uint64_t collisions;
    uint64_t mem_collisions;
    uint64_t dist_points;
    uint64_t number_steps_collect;
    uint64_t number_steps_locate;
    uint64_t dist_cols;
    int64_t cycles;
    bool success;
};

#include "templating/swig_helpers.inc"
typedef vOW<Point, Memory, RandomFunction, PRNG, Instance> vOWInstance;

Instance *instance_t(
    uint64_t _rad,
    uint64_t _nbits_state,
    uint64_t _mem_log_size,
    double _al,
    double _be,
    double _ga,
    unsigned long _prng_seed);
void *new_vow(Instance *instance);
void set_n_cores(Instance *instance, uint16_t n_cores);
void set_n_cores(void *vow, uint16_t n_cores);
void set_hansel_gretel(Instance *instance, bool hg, uint64_t max_crumbs);
void set_max_function_versions(void *vow, uint64_t mfv);
void set_initial_function_version(void *vow, uint64_t ifv);
void set_prng_seed(Instance *instance, unsigned long seed);
void set_prng_seed(void *vow, unsigned long seed);
unsigned long get_prng_seed(Instance *instance);
unsigned long get_prng_seed(void *vow);
void set_collect_vow_stats(Instance *instance, bool cvs);
void set_collect_vow_stats(void *vow, bool cvs);
void process_instance(Instance *instance);
void process_instance(void *vow);
void run_vow(void *vow);
void reset_vow(void *vow);
void delete_vow(void *vow);
void reseed_vow(void *vow, uint64_t seed);
void read_stats_vow(void *vow, statistics_t *stats);
