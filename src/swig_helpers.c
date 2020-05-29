#include <string.h>
#include <stdio.h>
#include "types/state.h"

#if defined(VOW_SIDH)
#elif defined(VOW_SIKE)
#else
#endif

#if defined(VOW_SIDH) || defined(VOW_SIKE)
void print_felm(felm_t *el)
{
    for (uint16_t i = 0; i < NWORDS_FIELD; i++)
        printf("%" PRIu64 " ", ((digit_t *)el)[i]);
    printf("\n");
}

void print_f2elm(felm_t *el)
{
    print_felm(&el[0]);
    print_felm(&el[1]);
}

void load_f2elm(felm_t *target, unsigned long long *in, int len)
{
    for (int i = 0; i < len; i++)
        ((digit_t *)target)[(i / NWORDS_FIELD) * NWORDS_FIELD + (i % NWORDS_FIELD)] = in[i];
}

void load_E(instance_t *inst, CurveAndPointsSIDH *E0, CurveAndPointsSIDH *E1)
{
    inst->E[0] = *E0;
    inst->E[1] = *E1;
}

void print_E(instance_t *inst)
{
    print_f2elm(inst->E[0].a24);
    print_f2elm(inst->E[0].xp);
    print_f2elm(inst->E[0].xq);
    print_f2elm(inst->E[0].xpq);
    printf(" at %llx\n", (long long unsigned int)&(inst->E[0]));
    print_f2elm(inst->E[1].a24);
    print_f2elm(inst->E[1].xp);
    print_f2elm(inst->E[1].xq);
    print_f2elm(inst->E[1].xpq);
    printf(" at %llx\n", (long long unsigned int)&(inst->E[1]));
}

CurveAndPointsSIDH *digit_t_to_CurveAndPointsSIDH_ptr(digit_t *ptr)
{
    return (CurveAndPointsSIDH *)ptr;
}
#endif

void reset_shared_state(shared_state_t *S)
{
#ifdef STORE_IN_MEMORY
    for (unsigned int i = 0; i < S->MEMORY_SIZE; i++)
    {
        S->memory[i].current_state.words[0] = 0;
        S->memory[i].initial_state.words[0] = 0;
        S->memory[i].current_steps = 0;
    }
#endif

    S->success = false;
    S->wall_time = 0.;
    S->total_time = 0.;
    S->final_avg_random_functions = 0.;
    S->collisions = 0;
    S->mem_collisions = 0;
    S->dist_points = 0;
    S->number_steps_collect = 0;
    S->number_steps_locate = 0;
    S->number_steps = 0;
#ifdef COLLECT_DATABASE_STATS
    S->debug_stats[0] = S.debug_stats[1] = S.debug_stats[2] = S.debug_stats[3] = (double)0.;
#endif

    if (S->dist_cols.size != 0)
    {
        freeTree(S->dist_cols.root);
    }
    initTree(&S->dist_cols);

    /* resync */
    free(S->resync_cores);
    S->resync_cores = (uint8_t *)calloc(S->N_OF_CORES, sizeof(uint8_t));
}

int64_t cpu_cycles(void)
{ // Access system counter for benchmarking
#if (OS_TARGET == OS_WIN) && (TARGET == TARGET_AMD64 || TARGET == TARGET_x86)
    return __rdtsc();
#elif (OS_TARGET == OS_WIN) && (TARGET == TARGET_ARM)
    return __rdpmccntr64();
#elif (OS_TARGET == OS_LINUX) && (TARGET == TARGET_AMD64 || TARGET == TARGET_x86)
    unsigned int hi, lo;

    asm volatile("rdtsc\n\t"
                 : "=a"(lo), "=d"(hi));
    return ((int64_t)lo) | (((int64_t)hi) << 32);
#elif (OS_TARGET == OS_LINUX) && (TARGET == TARGET_ARM || TARGET == TARGET_ARM64)
    struct timespec time;

    clock_gettime(CLOCK_REALTIME, &time);
    return (int64_t)(time.tv_sec * 1e9 + time.tv_nsec);
#else
    return 0;
#endif
}
