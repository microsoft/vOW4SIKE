/********************************************************************************************
* SIDH: an efficient supersingular isogeny cryptography library
*
* Abstract: benchmarking/testing functions for van Oorschot-Wiener attack against SIDH
*********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "test_extras.h"

#define VOW_SIDH

#include "../src/sidh_vow_base.h"
#include "../src/state.h"
#include "../src/bintree.h"

#define ATTACK_NAME "vOW_SIDH"
#define BENCH_LOOPS 1

#include "sidh_vow_instances.c"
#include "db_endpoints.c"

// greek letters
#if (OS_TARGET == OS_WIN)
#define _ALPHA_CHAR "%c", 224
#define _BETA_CHAR "%c", 225
#define _GAMMA_CHAR "%c", 226
#elif (OS_TARGET == OS_LINUX)
#include <locale.h>
#define _ALPHA_CHAR ("α")
#define _BETA_CHAR ("β")
#define _GAMMA_CHAR ("γ")
#endif

int stats_vow(uint64_t n_cores, bool precomp, bool hansel_gretel, bool collect_stats, uint64_t max_crumbs)
{
    unsigned int i, j;
    uint64_t collisions, mem_collisions, dist_points, number_steps_collect, number_steps_locate, number_steps, dist_cols;
    bool success;
    double calendar_time, total_time, random_functions;
    unsigned long long cycles, cycles1, cycles2;
    shared_state_t S;
#ifdef STORE_IN_DATABASE
    db_settings_t db_settings = load_db_settings();
#endif
#if (OS_TARGET == OS_LINUX)
    // on linux, set utf8 support
    setlocale(LC_ALL, "");
#endif

    printf("\nRunning vOW attack on SIDH");
    printf("\n----------------------------------------------------------------------------------------\n");

    for (i = 0; i < NUM_INSTS_STATS; i++)
    {
        success = true;
        random_functions = 0;
        collisions = 0;
        mem_collisions = 0;
        dist_points = 0;
        number_steps_collect = 0;
        number_steps_locate = 0;
        number_steps = 0;
        dist_cols = 0;
        calendar_time = 0;
        total_time = 0;
        cycles = 0;

        // Setting number of cores
        insts_stats[i].N_OF_CORES = (uint16_t)n_cores;
        // Setting precomputation
        insts_stats[i].PRECOMP = precomp;
        // Setting Hansel&Gretel optimization
        insts_stats[i].HANSEL_GRETEL = hansel_gretel;
        insts_stats[i].MAX_CRUMBS = max_crumbs;

        init_shared_state(insts_stats + i, &S
#ifdef STORE_IN_DATABASE
                          ,
                          &db_settings
#endif
        );

        printf("\n----------------------------------------------------------------------------------------\n");
        printf("\nInstance:\t");
        printf("n = %" PRIu64 "\t    ", (uint64_t)insts_stats[i].NBITS_K + 1);
        printf("w = %" PRIu64 "\t", (uint64_t)insts_stats[i].MEMORY_LOG_SIZE);
        printf(_ALPHA_CHAR);
        printf(" = %.2f\t", insts_stats[i].ALPHA);
        printf(_BETA_CHAR);
        printf(" = %.2f\t", insts_stats[i].BETA);
        printf(_GAMMA_CHAR);
        printf(" = %.2f\t", insts_stats[i].GAMMA);
        printf("modulus = %s", insts_stats[0].MODULUS);
        printf("\n\n");
        printf("Number of iterations averaged over: \t\t%" PRIu64 "\n", (uint64_t)BENCH_LOOPS);
        printf("Memory: \t\t\t\t\t");
#ifdef STORE_IN_DATABASE
        printf("DB\n");
#else
        printf("RAM\n");
#endif
        printf("Number of cores: \t\t\t\t%" PRIu64 "\n", n_cores);
        printf("Hansel & Gretel: \t\t\t\t%s", hansel_gretel ? "Yes" : "No");
        if (hansel_gretel)
            printf(", %" PRIu64 " crumbs\n", insts_stats[i].MAX_CRUMBS);
        else
            printf("\n");
        printf("Statistics only: \t\t\t\t");
        printf("%s\n", collect_stats ? "Yes (only running one function version)" : "No");

        S.MAX_FUNCTION_VERSIONS = 100000;
        S.collect_vow_stats = collect_stats;
        for (j = 0; j < BENCH_LOOPS; j++)
        {
            reset_shared_state(&S);

            S.initial_function_version += 1; /* Maintain across examples, but change */
            S.PRNG_SEED += j;                /* Different PRNG seed.. */

            cycles1 = cpucycles();
            vOW(&S);
            cycles2 = cpucycles();
            cycles = cycles + (cycles2 - cycles1);

            success &= S.success;
            random_functions += S.final_avg_random_functions;
            collisions += S.collisions;
            mem_collisions += S.mem_collisions;
            dist_points += S.dist_points;
            number_steps_collect += S.number_steps_collect;
            number_steps_locate += S.number_steps_locate;
            number_steps = number_steps_collect + number_steps_locate;
            dist_cols += S.dist_cols.size;
            calendar_time += S.wall_time;
            total_time += S.total_time;

#ifdef COLLECT_DATABASE_STATS
            printf("\n");
            printf("avg read  time: %.6f / %.1f = %.6f\n", S.debug_stats[1], S.debug_stats[0], S.debug_stats[1] / S.debug_stats[0]);
            printf("avg write time: %.6f / %.1f = %.6f\n", S.debug_stats[3], S.debug_stats[2], S.debug_stats[3] / S.debug_stats[2]);
            printf("\n");
#endif

            if (!collect_stats)
            {
                printf("\n Iteration %d", j);
                if (S.success)
                    printf(" COMPLETED using about %.2f random functions and %.2f seconds", S.final_avg_random_functions, S.wall_time);
                else
                    printf(" INCOMPLETE. Used about %.2f random functions and %.2f seconds", S.final_avg_random_functions, S.wall_time);
            }
        }
        free_shared_state(&S);

        if (collect_stats)
        {
            printf("\nNumber of function iterations (i): \t\t%.2f\n", (double)number_steps / (double)random_functions);
            printf("\t For collecting dist. points: \t\t%.2f (%.2f%%)\n",
                   ((double)number_steps_collect / (double)random_functions),
                   100 * ((double)number_steps_collect / (double)number_steps));
            printf("\t For locating collisions: \t\t%.2f (%.2f%%)\n",
                   ((double)number_steps_locate / (double)random_functions),
                   100 * ((double)number_steps_locate / (double)number_steps));
            printf("Number of collisions per function: \t\t%.2f (expected 1.3w = %.2f, ratio = %.2f)\n", ((double)collisions / (double)random_functions),
                   1.3 * pow(2, (double)insts_stats[i].MEMORY_LOG_SIZE),
                   ((double)collisions / (double)random_functions) / (1.3 * pow(2, (double)insts_stats[i].MEMORY_LOG_SIZE)));
            printf("Number of distinct collisions per function (c): %.2f (expected 1.1w = %.2f, ratio = %.2f)\n",
                   ((double)dist_cols / (double)random_functions),
                   1.1 * pow(2, (double)insts_stats[i].MEMORY_LOG_SIZE),
                   ((double)dist_cols / (double)random_functions) / (1.1 * pow(2, (double)insts_stats[i].MEMORY_LOG_SIZE)));
            printf("\n");
            printf("Expected number of function versions (n/(2c)): \t%.2f (expected 0.45n/w = %.2f, ratio = %.2f)\n",
                   3 * pow(2, (double)insts_stats[i].NBITS_K + 1) / (2 * ((double)dist_cols / (double)random_functions)),
                   0.45 * 3 * pow(2, (double)insts_stats[i].NBITS_K + 1) / pow(2, (double)insts_stats[i].MEMORY_LOG_SIZE),
                   (3 * pow(2, (double)insts_stats[i].NBITS_K + 1) / (2 * ((double)dist_cols / (double)random_functions))) /
                       (0.45 * 3 * pow(2, (double)insts_stats[i].NBITS_K + 1) / pow(2, (double)insts_stats[i].MEMORY_LOG_SIZE)));
            printf("Expected total run-time (in/(2c)): \t\t%.2f (expected %cn^3/w = %.2f, ratio = %.2f)\n",
                   ((double)number_steps / (double)random_functions) * 3 * pow(2, (double)insts_stats[i].NBITS_K + 1) / (2 * ((double)dist_cols / (double)random_functions)),
                   251,
                   sqrt(pow(3 * pow(2, (double)insts_stats[i].NBITS_K + 1), 3) / pow(2, insts_stats[i].MEMORY_LOG_SIZE)),
                   (((double)number_steps / (double)random_functions) * 3 * pow(2, (double)insts_stats[i].NBITS_K + 1) / (2 * ((double)dist_cols / (double)random_functions))) / sqrt(pow(3 * pow(2, (double)insts_stats[i].NBITS_K + 1), 3) / pow(2, insts_stats[i].MEMORY_LOG_SIZE)));
        }
        else
        {
            printf("\nAll tests successful: \t\t\t%s\n\n", success ? "Yes" : "No");
            printf("\n");
            printf("Number of function iterations: \t\t%.2f (expected sqrt(n^3/w) = %.2f, ratio = %.2f)\n",
                   (double)number_steps / (double)BENCH_LOOPS,
                   sqrt(pow(3 * pow(2, (double)insts_stats[i].NBITS_K + 1), 3) / pow(2, insts_stats[i].MEMORY_LOG_SIZE)),
                   ((double)number_steps / (double)BENCH_LOOPS) / (sqrt(pow(3 * pow(2, (double)insts_stats[i].NBITS_K + 1), 3) / pow(2, insts_stats[i].MEMORY_LOG_SIZE))));
            printf("\t For collecting dist. points: \t%.2f (%.2f%%)\n",
                   ((double)number_steps_collect / (double)BENCH_LOOPS),
                   100 * ((double)number_steps_collect / (double)number_steps));
            printf("\t For locating collisions: \t%.2f (%.2f%%)\n",
                   ((double)number_steps_locate / (double)BENCH_LOOPS),
                   100 * ((double)number_steps_locate / (double)number_steps));
            printf("Number of function versions: \t\t%.2f (expected 0.45n/w = %.2f, ratio = %.2f)\n",
                   (double)random_functions / (double)BENCH_LOOPS,
                   0.45 * 3 * pow(2, (double)insts_stats[i].NBITS_K + 1) / pow(2, insts_stats[i].MEMORY_LOG_SIZE),
                   ((double)random_functions / (double)BENCH_LOOPS) /
                       (0.45 * 3 * pow(2, (double)insts_stats[i].NBITS_K + 1) / pow(2, insts_stats[i].MEMORY_LOG_SIZE)));
            printf("Number of collisions per function: \t%.2f (expected 1.3w = %.2f, ratio = %.2f)\n",
                   ((double)collisions / (double)random_functions),
                   1.3 * pow(2, insts_stats[i].MEMORY_LOG_SIZE),
                   (((double)collisions / (double)random_functions)) / (1.3 * pow(2, insts_stats[i].MEMORY_LOG_SIZE)));
        }
        printf("\nTotal time                        : %.2f", total_time);
        printf("\nTotal time    (avg per iteration) : %.2f", total_time / BENCH_LOOPS);
        printf("\nCalendar time                     : %.2f", calendar_time);
        printf("\nCalendar time (avg per iteration) : %.2f", calendar_time / BENCH_LOOPS);
        printf("\nCycle count   (avg per iteration) : %lld\n\n", cycles / BENCH_LOOPS);
    }

#ifdef STORE_IN_DATABASE
    free(db_settings.db_endpoints);
#endif

    return 0;
}

int main(int argc, char **argv)
{
    int Status = PASSED;
    uint64_t n_cores = 1;       // One core by default
    bool precomp = false;       // Precomputation is disabled by default
    bool hansel_gretel = false; // Hansel&Gretel optimization is disabled by default
    uint64_t max_crumbs = 10;
    bool collect_stats = false; // Extra collection of stats is disabled by default
    bool help_flag = false;
    int MAX_ARGSplus1 = 7; // Current format: "test_vOW_SIDH -n [N_CORES] -p -H -s -h"

    // avoid output buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc > MAX_ARGSplus1)
    {
        help_flag = true;
        goto help;
    }

    for (int i = 0; i < argc - 1; i++)
    {
        if (argv[i + 1][0] != '-')
        {
            help_flag = true;
            goto help;
        }
        switch (argv[i + 1][1])
        {
        case 'n':
            n_cores = strtol(argv[i + 2], NULL, 10);
            i++;
            if (n_cores > 1000)
                help_flag = true;
            break;
        case 'p':
            precomp = true;
            break;
        case 'H':
            hansel_gretel = true;
            max_crumbs = strtol(argv[i + 2], NULL, 10);
            i++;
            break;
        case 's':
            collect_stats = true;
            break;
        case 'h':
            help_flag = true;
            break;
        default:
            help_flag = true;
            break;
        }
        if (help_flag)
        {
            goto help;
        }
    }

    Status = stats_vow(n_cores, precomp, hansel_gretel, collect_stats, max_crumbs); // Testing
    if (Status != PASSED)
    {
        printf("\n\n   Error detected while running attack... \n\n");
        return 1;
    }

help:
    if (help_flag)
    {
        printf("\n Usage:");
        printf("\n test_vOW_SIDH -n [N_CORES] -p -H [MAX_CRUMBS] -s -h \n");
        printf("\n -n : number of cores (one core by default). Maximum 1000 cores.");
        printf("\n -p : precomputation on (off by default).");
        printf("\n -H : Hansel&Gretel optimization on (off by default).");
        printf("\n -s : collection of attack stats on (off by default).");
        printf("\n -h : this help.\n\n");
    }

#ifdef _WIN64
    system("PAUSE");
#endif

    return Status;
}
