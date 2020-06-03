#pragma once
#include "../types/instance.hpp"
#include <cstdint>

class GenInstance : public IInstance
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

    void process();
    GenInstance(GenInstance *inst);
    GenInstance(
        uint64_t _rad,
        uint64_t _nbits_state,
        uint64_t _mem_log_size,
        double _al,
        double _be,
        double _ga,
        unsigned long _prng_seed
    ) : RAD(_rad),
        NBITS_STATE(_nbits_state),
        MEMORY_LOG_SIZE(_mem_log_size),
        ALPHA(_al),
        BETA(_be),
        GAMMA(_ga),
        PRNG_SEED(_prng_seed)
        {};
};
