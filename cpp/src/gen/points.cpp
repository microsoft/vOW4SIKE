#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "points.hpp"
#include "../prng/xof.h"
#include "../utils/fix_overflow.h"
#include "../types/state.hpp"
#include "../types/instance.hpp"
#include "../utils/buftools.h"

void GenPoint::initialize(GenInstance *_instance)
{
    instance = _instance;
    if (instance->NWORDS_STATE == 0)
    {
        point_exception st_ex;
        throw st_ex;
    }
    bytes = (unsigned char *)calloc(instance->NWORDS_STATE, sizeof(digit_t));
    assert(bytes != NULL);
}

GenPoint::GenPoint(GenInstance *_instance)
{
    initialize(_instance);
}

GenPoint::GenPoint(GenPoint *_point)
{
    initialize(_point->instance);
    from_point(*_point);
}

GenPoint::~GenPoint()
{
    free(bytes);
}

void GenPoint::from_point(const IPoint &s)
{
    bufcpy(bytes, s.bytes, instance->NBYTES_STATE);
}

void GenPoint::to_buf(digit_t *r)
{
    bufcpy(r, bytes, instance->NBYTES_STATE);
}

void GenPoint::from_buf(const digit_t *s)
{
    bufcpy(bytes, s, instance->NBYTES_STATE);
}

void GenPoint::print()
{
    uint64_t i;

    if (bytes == NULL)
    {
        for (i = 0; i < instance->NBYTES_STATE; i++)
        {
            printf("--");
        }
        return;
    }
    for (i = 0; i < instance->NBYTES_STATE; i++)
    {
        printf("%02x", bytes[i]);
    }
}

bool GenPoint::operator==(const IPoint &other) const
{
    uint64_t i;
    uint64_t nwords_minus_one = instance->NBYTES_STATE / sizeof(digit_t);

    // first check using word-sized values as far as possible
    for (i = 0; i < nwords_minus_one; i++)
    {
        if (((digit_t *)bytes)[i] != ((digit_t *)other.bytes)[i])
        {
            return false;
        }
    }

    // then check for remaning bytes
    for (i = nwords_minus_one * sizeof(digit_t); i < instance->NBYTES_STATE; i++)
    {
        if (bytes[i] != other.bytes[i])
        {
            return false;
        }
    }

    return true;
}

bool GenPoint::operator!=(const IPoint &other) const
{
    return !(*this == other);
}

static void get_val(uint64_t *val, uint64_t NWORDS_STATE, digit_t *state_words, uint64_t MEMORY_LOG_SIZE)
{
    /* Use as many bits as possible, with a max of 64 (word size) 
     * This limits the precision of distinguishedness checking to 64 bits
     */
    if (NWORDS_STATE == 1) {
        *val = state_words[0] >> MEMORY_LOG_SIZE;
    }
    else {
        *val = (state_words[0] >> MEMORY_LOG_SIZE) | (state_words[1] << (64-MEMORY_LOG_SIZE));
    }
}

static void add_fv(uint64_t *val, const uint64_t fv, const uint64_t db)
{
    *val = *val + fv*db;
}

/**
 * @brief Evaluates whether a point is distinguished
 *
 * @return true point is distinguished
 * @return false point is not distinguished
 */
bool GenPoint::is_distinguished(uint64_t function_version)
{
    /* Divide distinguishedness over interval to avoid bad cases */
    uint64_t val;

    get_val(&val, instance->NWORDS_STATE, (digit_t *)bytes, instance->MEMORY_LOG_SIZE);
    add_fv(&val, function_version, (uint64_t)instance->DIST_BOUND);
    /* Zero top bits if < 64 bits filled */
    if ((instance->NBITS_STATE - instance->MEMORY_LOG_SIZE) <= 64) {
        val &= (((uint64_t)1 << (instance->NBITS_STATE - instance->MEMORY_LOG_SIZE)) - 1);
    }
    return (val <= instance->DIST_BOUND);
}

/**
 * @brief Recovers memory index for storing current distinguished point
 *
 * @return uint64_t
 */
uint64_t GenPoint::mem_index(uint64_t random_functions) 
{
    // returns   (lob + function) mod memsize
    return (uint64_t)((words[0] + random_functions) & instance->MEMORY_SIZE_MASK);
}

/**
 * @brief Samples a new starting point
 *
 * @param private_state pointer
 */
void GenPoint::sample(IPRNG *prng)
{
    prng->sample(bytes, (unsigned long)instance->NBYTES_STATE);
    fix_overflow(bytes, instance->NBYTES_STATE, instance->NBITS_OVERFLOW);
}

void GenPoint::sample(uint64_t seed)
{
    // if we are provided just a seed, it means we want to have all states sample
    // the same randomness (i.e. we are sampling golden preimages for a random function)
    // hence, use the XOF, but salt it with an offset of the seed, so we avoid weird interactions.
    // having alternative XOFs would also work
    XOF(bytes, (unsigned char*)&seed, (unsigned long)instance->NBYTES_STATE, sizeof(seed), 0xdeadbeef);
    fix_overflow(bytes, instance->NBYTES_STATE, instance->NBITS_OVERFLOW);
}

void GenPoint::reset()
{
    memset(bytes, 0, sizeof(digit_t) * instance->NWORDS_STATE);
}
