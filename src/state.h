#pragma once
#include "types/triples.h"
#include "types/state.h"
#include "types/instance.h"
#include "settings.h"

#if defined(VOW_SIKE) || defined(VOW_SIDH)
#include "curve_math.h" // to get CurveandPointsSIDH
#endif

/* Initializing the state */
void init_shared_state(instance_t *inst, shared_state_t *S
#ifdef STORE_IN_DATABASE
                       ,
                       db_settings_t *db_settings
#endif
);
void init_private_state(shared_state_t *shared_state, private_state_t *private_state);
void free_shared_state(shared_state_t *S);
void free_private_state(private_state_t *private_state);
void clean_private_state(private_state_t *private_state);
void print_shared_state(shared_state_t *S);
void reset_shared_state(shared_state_t *S);
