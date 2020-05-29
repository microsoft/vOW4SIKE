#pragma once
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "state.h"
#include "curve_math.h"

#if defined(VOW_SIDH)
#define EXTRA_MEM_LOG_SIZE 3
#elif defined(VOW_SIKE)
#define EXTRA_MEM_LOG_SIZE 0
#endif

/* Functions for vOW */
void UpdateStSIDH(unsigned char jinvariant[FP2_ENCODED_BYTES], st_t *r, const st_t *s, private_state_t *private_state);
bool DistinguishedSIDH(private_state_t *private_state);
uint64_t MemIndexSIDH(private_state_t *private_state);
void SampleSIDH(private_state_t *private_state);
void UpdateSIDH(private_state_t *private_state);
void UpdateRandomFunctionSIDH(shared_state_t *S, private_state_t *private_state);
bool BacktrackSIDH(trip_t *c0, trip_t *c1, shared_state_t *S, private_state_t *private_state);
bool vOW(shared_state_t *S);

#define distinguished           DistinguishedSIDH
#define mem_index               MemIndexSIDH
#define sample                  SampleSIDH
#define backtrack               BacktrackSIDH
#define update                  UpdateSIDH
#define update_random_function  UpdateRandomFunctionSIDH

// Functions for swig interface
void print_felm(felm_t *el);
void print_f2elm(felm_t *el);
void load_f2elm(felm_t *target, unsigned long long *in, int len);
void load_E(instance_t *inst, CurveAndPointsSIDH *E0, CurveAndPointsSIDH *E1);
void print_E(instance_t *inst);
CurveAndPointsSIDH *digit_t_to_CurveAndPointsSIDH_ptr(digit_t *ptr);
int64_t cpu_cycles(void);
