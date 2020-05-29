
void print_shared_state(shared_state_t *S)
{
    printf("\nNBITS_STATE %" PRIu64, S->NBITS_STATE);
    printf("\nNBYTES_STATE %" PRIu64, S->NBYTES_STATE);
    printf("\nNWORDS_STATE %" PRIu64, S->NWORDS_STATE);
    printf("\nNBITS_OVERFLOW %" PRIu64, S->NBITS_OVERFLOW);

    printf("\nMEMORY_LOG_SIZE %" PRIu64, S->MEMORY_LOG_SIZE);
    printf("\nMEMORY_SIZE %" PRIu64, S->MEMORY_SIZE);
    printf("\nMEMORY_SIZE_MASK %" PRIu64, S->MEMORY_SIZE_MASK);
    printf("\nMAX_STEPS %.2f", S->MAX_STEPS);
    printf("\nMAX_DIST %" PRIu64, S->MAX_DIST);
    printf("\nMAX_FUNCTION_VERSIONS %" PRIu64, S->MAX_FUNCTION_VERSIONS);
    printf("\nDIST_BOUND %.2f", S->DIST_BOUND);

    printf("\nN_OF_THREADS %" PRIu16, S->N_OF_CORES);

#if (defined(VOW_SIDH) || defined(VOW_SIKE))
    printf("\ndelta %lu", S->delta);
    printf("\nlenstrat %lu", S->lenstrat);
#endif

#if defined(VOW_SIDH)
    printf("\nPRECOMP %d", (int)S->PRECOMP);
#endif

    printf("\ninitial_function_version %" PRIu64, S->initial_function_version);
    printf("\nfinal_avg_random_functions %.2f", S->final_avg_random_functions);
    printf("\ncollect_vow_stats %d", (int)S->collect_vow_stats);
    printf("\nsuccess %d", (int)S->success);
    printf("\nwall_time %.2f", S->wall_time);
    printf("\ncollisions %" PRIu64, S->collisions);
    printf("\nmem_collisions %" PRIu64, S->mem_collisions);
    printf("\ndist_points %" PRIu64, S->dist_points);
    printf("\nnumber_steps %" PRIu64, S->number_steps);

    printf("\nPRNG_SEED %lu", S->PRNG_SEED);
}

void init_private_state(shared_state_t *S, private_state_t *private_state)
{
    private_state->thread_id = omp_get_thread_num();

    private_state->current_dist = 0;
    private_state->random_functions = 1;
    private_state->function_version = S->initial_function_version;

    private_state->NBITS_OVERFLOW = S->NBITS_OVERFLOW;
    private_state->NWORDS_STATE = S->NWORDS_STATE;
    private_state->NBYTES_STATE = S->NBYTES_STATE;
    private_state->NBITS_STATE = S->NBITS_STATE;

    private_state->DIST_BOUND = S->DIST_BOUND;
    private_state->MAX_DIST = S->MAX_DIST;
    private_state->MAX_FUNCTION_VERSIONS = S->MAX_FUNCTION_VERSIONS;
    private_state->MAX_STEPS = S->MAX_STEPS;
    private_state->MEMORY_LOG_SIZE = S->MEMORY_LOG_SIZE;
    private_state->MEMORY_SIZE = S->MEMORY_SIZE;
    private_state->MEMORY_SIZE_MASK = S->MEMORY_SIZE_MASK;

#if defined(VOW_SIDH) || defined(VOW_SIKE)

#if defined(VOW_SIDH)

    private_state->PRECOMP = S->PRECOMP;
    fp2copy(S->E[0].a24, private_state->E[0].a24);
    fp2copy(S->E[0].xp, private_state->E[0].xp);
    fp2copy(S->E[0].xq, private_state->E[0].xq);
    fp2copy(S->E[0].xpq, private_state->E[0].xpq);
    fp2copy(S->E[1].a24, private_state->E[1].a24);
    fp2copy(S->E[1].xp, private_state->E[1].xp);
    fp2copy(S->E[1].xq, private_state->E[1].xq);
    fp2copy(S->E[1].xpq, private_state->E[1].xpq);

#elif defined(VOW_SIKE)

    private_state->E[0] = S->E[0];
    private_state->E[1] = S->E[1];
    private_state->external_E[0] = S->external_E[0];
    private_state->external_E[1] = S->external_E[1];

#endif

    private_state->delta = S->delta;
    private_state->lenstrat = S->lenstrat;
    private_state->strat = calloc(private_state->lenstrat, sizeof(unsigned long));
    for (unsigned long i = 0; i < private_state->lenstrat; i++)
        private_state->strat[i] = S->strat[i];
    fp2copy(S->jinv, private_state->jinv);
    private_state->jinvLUT = S->jinvLUT;

#else

    private_state->image = S->image;
    private_state->preimages[0] = S->preimages[0];
    private_state->preimages[1] = S->preimages[1];

#endif

    private_state->collect_vow_stats = S->collect_vow_stats;
    private_state->collisions = 0;
    private_state->mem_collisions = 0;
    private_state->dist_points = 0;
    private_state->number_steps_collect = 0;
    private_state->number_steps_locate = 0;

    private_state->current.current_state.words = calloc(private_state->NWORDS_STATE, sizeof(digit_t));
    private_state->current.initial_state.words = calloc(private_state->NWORDS_STATE, sizeof(digit_t));
    private_state->current.current_steps = 0;
    private_state->trip.current_state.words = calloc(private_state->NWORDS_STATE, sizeof(digit_t));
    private_state->trip.initial_state.words = calloc(private_state->NWORDS_STATE, sizeof(digit_t));
    private_state->trip.current_steps = 0;

    /* PRNG */
    XOF((unsigned char *)(&(private_state->PRNG_SEED)), (unsigned char *)(&S->PRNG_SEED), sizeof(private_state->PRNG_SEED), sizeof(S->PRNG_SEED), (unsigned long)omp_get_thread_num() + 1);
    init_prng(&private_state->prng_state, (unsigned long)private_state->PRNG_SEED);

    /* Initialization for Hansel&Gretel */
    private_state->HANSEL_GRETEL = S->HANSEL_GRETEL;
    private_state->crumbs.swap_position = 0;
    private_state->crumbs.scratch_position = 0;
    private_state->crumbs.index_position = 0;
    private_state->crumbs.max_dist = 1;
    private_state->crumbs.real_dist = 1;
    private_state->crumbs.position = 0;
    private_state->crumbs.num_crumbs = 0;
    private_state->crumbs.max_crumbs = S->MAX_CRUMBS;
    private_state->crumbs.positions = calloc(1, private_state->crumbs.max_crumbs * sizeof(uint64_t));
    private_state->crumbs.index_crumbs = calloc(1, private_state->crumbs.max_crumbs * sizeof(uint64_t));
    private_state->crumbs.crumbs = calloc(private_state->NWORDS_STATE, private_state->crumbs.max_crumbs * sizeof(uint64_t));
}

void clean_private_state(private_state_t *private_state)
{
    /* Initialization for Hansel&Gretel */
    private_state->crumbs.swap_position = 0;
    private_state->crumbs.scratch_position = 0;
    private_state->crumbs.index_position = 0;
    private_state->crumbs.max_dist = 1;
    private_state->crumbs.real_dist = 1;
    private_state->crumbs.position = 0;
    private_state->crumbs.num_crumbs = 0;
}

void free_private_state(private_state_t *private_state)
{
    free_trip(&private_state->current);
    free_trip(&private_state->trip);
    free(private_state->crumbs.positions);
    free(private_state->crumbs.index_crumbs);
    free(private_state->crumbs.crumbs);
#if defined(VOW_SIDH) || defined(VOW_SIKE)
    free(private_state->strat);
#endif
}
