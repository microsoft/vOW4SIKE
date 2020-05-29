
/**
 * @brief find a number of points and measure how long it takes on the avaliable core
 * 
 */
void run_benchmark(
    double *points_ratio,
    instance_t *instance,
    uint64_t target_number_of_points)
{
#ifdef RUN_ACTUAL_BENCHMARKING
    double total_time = 0.;
    double *benchmark;
    shared_state_t benchmark_S;
#ifdef STORE_IN_DATABASE
    db_settings_t db_settings = load_db_settings();
#endif

    // Init a state from an instance, and set the number of threads to 1
    benchmark_S = init_shared_state(instance
#ifdef STORE_IN_DATABASE
                                    ,
                                    &db_settings
#endif
    );
    benchmark_S.N_OF_CORES = 1;

    // Stop at a certain number of distinguished points!

#if !(defined(VOW_SIKE) || defined(VOW_SIDH))
    prng_state_t prng_state;
    init_prng(&prng_state, benchmark_S.PRNG_SEED);
    sample_prng(&prng_state, benchmark_S.image.bytes, (unsigned long)benchmark_S.NBYTES_STATE);
    fix_overflow(&benchmark_S.image, benchmark_S.NBYTES_STATE, benchmark_S.NBITS_OVERFLOW);
#endif

    // Run benchmark

    // Explicitly disable dynamic teams
    omp_set_dynamic(0);

    // Allocate memory for benchmark
    benchmark = (double *)malloc(instance->N_OF_CORES * sizeof(double));
    if (benchmark == NULL) {
        fprintf(stderr, "error: could not allocate memory for the benchmarks");
        assert(0);
    }

    // run benchmark on each core
#pragma omp parallel num_threads(instance->N_OF_CORES)
    {
        int thread_id = omp_get_thread_num();
        bool success;
        private_state_t private_state = init_private_state(&benchmark_S);
        initialize_private_memory(&benchmark_S, &private_state);
        trip_t t = init_trip(private_state.NWORDS_STATE);

        double wall_time = omp_get_wtime();
        for (uint64_t i = 0; i < target_number_of_points; i++) {
            (void)vOW_one_iteration(&benchmark_S, &private_state, &t, &success, 1.);
        }
        wall_time = omp_get_wtime() - wall_time;

        // Save result
        benchmark[thread_id] = wall_time;
#pragma omp atomic
        total_time += wall_time;

        // Free memory
        free_trip(&t);
        cleanup_private_memory(&private_state);
        free_private_state(&private_state);
    }

    // Send benchmark information to database and recover ratios
#ifdef STORE_IN_DATABASE
    fprintf(stderr, "error: rrprf_from_benchmark not implemented\n");
#else
    for (uint64_t i = 0; i < instance->N_OF_CORES; i++) {
        points_ratio[i] = benchmark[i] / total_time;
    }
#endif

    // Free benchmark_S without freeing the buffers
    free(benchmark);
    free_shared_state(&benchmark_S);
#else
    (void)target_number_of_points;
    for (uint64_t i = 0; i < instance->N_OF_CORES; i++) {
        points_ratio[i] = 1. / (double)instance->N_OF_CORES;
    }
#endif
}
