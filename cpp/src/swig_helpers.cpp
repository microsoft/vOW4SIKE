#include "swig_helpers.hpp"

Instance *instance_t(
        uint64_t _rad,
        uint64_t _nbits_state,
        uint64_t _mem_log_size,
        double _al,
        double _be,
        double _ga,
        unsigned long _prng_seed)
{
    return new Instance(_rad, _nbits_state, _mem_log_size, _al, _be, _ga, _prng_seed);
}

void *new_vow(Instance *instance)
{
    return (void *) new vOWInstance(instance);
}

void set_n_cores(Instance *instance, uint16_t n_cores)
{
    instance->N_OF_CORES = n_cores;
}

void set_n_cores(void *vow, uint16_t n_cores)
{
    set_n_cores(((vOWInstance *)vow)->instance, n_cores);
}

void set_hansel_gretel(Instance *instance, bool hg, uint64_t max_crumbs)
{
    instance->HANSEL_GRETEL = hg;
    instance->MAX_CRUMBS = max_crumbs;
}

void set_max_function_versions(Instance *instance, uint64_t mfv)
{
    instance->MAX_FUNCTION_VERSIONS = mfv;
}

void set_max_function_versions(void *vow, uint64_t mfv)
{
    set_max_function_versions(((vOWInstance *)vow)->instance, mfv);
}

// available, but is overridden by instance->process() sampling a pseudo-random
// initial function version
void set_initial_function_version(void *vow, uint64_t ifv)
{
    ((vOWInstance *)vow)->instance->initial_function_version = ifv;
}

unsigned long get_prng_seed(Instance *instance)
{
    return instance->PRNG_SEED;
}

unsigned long get_prng_seed(void *vow)
{
    return get_prng_seed(((vOWInstance *)vow)->instance);
}

void set_prng_seed(Instance *instance, unsigned long seed)
{
    instance->PRNG_SEED = seed;
}

void set_prng_seed(void *vow, unsigned long seed)
{
    set_prng_seed(((vOWInstance *)vow)->instance, seed);
}

void set_collect_vow_stats(Instance *instance, bool cvs)
{
    instance->collect_vow_stats = cvs;
}

void set_collect_vow_stats(void *vow, bool cvs)
{
    set_collect_vow_stats(((vOWInstance *)vow)->instance, cvs);
}

void process_instance(Instance *instance)
{
    instance->process();
}

void process_instance(void *vow)
{
    process_instance(((vOWInstance *)vow)->instance);
}

void reset_vow(void *vow)
{
    ((vOWInstance *)vow)->reset();
}

void run_vow(void *vow)
{
    ((vOWInstance *)vow)->run();
}

void delete_vow(void *vow)
{
    delete (vOWInstance *)vow;
}

void reseed_vow(void *vow, uint64_t seed)
{
    ((vOWInstance *)vow)->instance->PRNG_SEED = (unsigned long) seed;
    ((vOWInstance *)vow)->instance->process();
}

void read_stats_vow(void *vow, statistics_t *stats)
{
    stats->calendar_time = ((vOWInstance *)vow)->wall_time;
    stats->total_time = ((vOWInstance *)vow)->total_time;
    stats->random_functions = ((vOWInstance *)vow)->final_avg_random_functions;
    stats->collisions = ((vOWInstance *)vow)->collisions;
    stats->mem_collisions = ((vOWInstance *)vow)->mem_collisions;
    stats->dist_points = ((vOWInstance *)vow)->dist_points;
    stats->number_steps_collect = ((vOWInstance *)vow)->number_steps_collect;
    stats->number_steps_locate = ((vOWInstance *)vow)->number_steps_locate;
    stats->dist_cols = ((vOWInstance *)vow)->dist_cols.size;
    stats->cycles = ((vOWInstance *)vow)->cycles;
    stats->success = ((vOWInstance *)vow)->success;
};

#include "templating/swig_helpers.inc"