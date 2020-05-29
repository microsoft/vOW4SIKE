#pragma once
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "state.h"

/* Functions for vOW */

bool DistinguishedGen(private_state_t * private_state);
uint64_t MemIndexGen(private_state_t * private_state);
void SampleGen(private_state_t * private_state);
void UpdateGen(private_state_t * private_state);
void UpdateRandomFunctionGen(shared_state_t *S, private_state_t * private_state);
bool BacktrackGen(trip_t *c0, trip_t *c1, shared_state_t *S, private_state_t * private_state);
bool vOW(shared_state_t *S);

#define distinguished           DistinguishedGen
#define mem_index               MemIndexGen
#define sample                  SampleGen
#define backtrack               BacktrackGen
#define update                  UpdateGen
#define update_random_function  UpdateRandomFunctionGen

// Functions for swig interface
int64_t cpu_cycles(void);
