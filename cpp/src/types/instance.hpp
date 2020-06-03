#pragma once
#include <cstdint>
class IInstance
{
public:
    // parameters
    uint64_t RAD;
    uint64_t NBITS_STATE;
    uint64_t MEMORY_LOG_SIZE;
    double ALPHA;
    double BETA;
    double GAMMA;
    unsigned long PRNG_SEED;
    uint16_t N_OF_CORES;
    bool HANSEL_GRETEL;
    uint64_t MAX_CRUMBS;
    uint64_t initial_function_version;
    bool collect_vow_stats = false; // by default don't collect stats (=> terminate run when successful)

    // processed parameters
    uint64_t NBYTES_STATE;
    uint64_t NWORDS_STATE;
    uint64_t NBITS_OVERFLOW;
    uint64_t MEMORY_SIZE;
    uint64_t MEMORY_SIZE_MASK;
    double MAX_STEPS;
    uint64_t MAX_DIST;
    uint64_t MAX_FUNCTION_VERSIONS;
    double DIST_BOUND;
    double THETA;

    // status
    bool processed = false;

    virtual void process() = 0;
    virtual ~IInstance() {}
};
