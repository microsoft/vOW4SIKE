#pragma once
#include<cstdint>

class resync_state_t
{
    public:
    uint8_t *cores;
    uint64_t frequency;
    uint64_t function_version;
    uint64_t random_functions;
};
