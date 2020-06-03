#include <cstdio>
#include <cstring>
#include <cmath>
#define __STDC_FORMAT_MACROS
#include <cstdint>

// vow generic library
#include "vow.hpp"

// concrete mitm instance
#include "gen/points.hpp"
#include "gen/random_function.hpp"
#include "gen/instance.hpp"
#include "memory/ram.hpp"
#include "prng/aes.hpp"
#include "prng/lcg.hpp"
#include "utils/cycles.h"

// #include "../src/bintree.h"

#define ATTACK_NAME "vOW_gen"
#define BENCH_LOOPS 100

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


int stats_vow(uint64_t n_cores, bool hansel_gretel, bool collect_stats, uint64_t max_crumbs)
{
    double
        calendar_time = 0.,
        total_time = 0.,
        random_functions = 0.;
    unsigned long long
        cycles = 0,
        cycles1 = 0,
        cycles2 = 0;
    uint64_t
        collisions = 0,
        mem_collisions = 0,
        dist_points = 0,
        number_steps_collect = 0,
        number_steps_locate = 0,
        number_steps = 0,
        dist_cols = 0;
    bool
        success = true;

    #if (OS_TARGET == OS_LINUX)
    // on linux, set utf8 support
    setlocale(LC_ALL, "");
    #endif

    printf("\nRunning generic vOW attack\t");
    printf("\n----------------------------------------------------------------------------------------\n");

    #include "gen/instances.inc" // todo: irl use python


    for (int i = 0; i < NUM_INSTS_STATS; i++)
    {
        success = true;
        calendar_time = 0;
        total_time = 0;
        cycles = 0;
        random_functions = 0.;
        collisions = 0;
        mem_collisions = 0;
        dist_points = 0;
        number_steps_collect = 0;
        number_steps_locate = 0;
        number_steps = 0;
        dist_cols = 0;

        // Setting number of cores
        inst_stats[i]->N_OF_CORES = (uint16_t)n_cores;
        // Setting Hansel&Gretel optimization
        inst_stats[i]->HANSEL_GRETEL = hansel_gretel;
        inst_stats[i]->MAX_CRUMBS = max_crumbs;
        inst_stats[i]->process();

        // derive sizes fro minstance
        typedef GenInstance Instance;
        typedef GenPoint Point;
        typedef LocalMemory<Point, Instance> Memory;
        typedef AESPRNG PRNG;
        // typedef LCG PRNG;
        typedef GenRandomFunction<Point> RandomFunction;

        vOW<Point, Memory, RandomFunction, PRNG, Instance> vow(inst_stats[i]);

        printf("\n----------------------------------------------------------------------------------------\n");
        printf("\nInstance:\t");
        printf("n = %" PRIu64 "\t    ", vow.instance->NBITS_STATE);
        printf("w = %" PRIu64 "\t", vow.instance->MEMORY_LOG_SIZE);
        printf(_ALPHA_CHAR);
        printf(" = %.2f\t", vow.instance->ALPHA);
        printf(_BETA_CHAR);
        printf(" = %.2f\t", vow.instance->BETA);
        printf(_GAMMA_CHAR);
        printf(" = %.2f\t", vow.instance->GAMMA);
        printf("\n\n");
        printf("Number of iterations averaged over: \t\t%" PRIu64 "\n", (uint64_t)BENCH_LOOPS);
        printf("Memory: \t\t\t\t\t");
        printf("RAM\n");
        printf("Number of cores: \t\t\t\t%" PRIu64 "\n", n_cores);
        printf("Hansel & Gretel: \t\t\t\t%s", hansel_gretel ? "Yes" : "No");
        if (hansel_gretel)
            printf(", %" PRIu64 " crumbs\n", vow.instance->MAX_CRUMBS);
        else
            printf("\n");
        printf("Statistics only: \t\t\t\t%s\n", collect_stats ? "Yes (only running one function version)" : "No");

        vow.instance->MAX_FUNCTION_VERSIONS = 100000;
        vow.instance->collect_vow_stats = collect_stats;
        for (int j = 0; j < BENCH_LOOPS; j++)
        {
            // reset the statistics (& memory)
            vow.reset();
            vow.instance->PRNG_SEED += (unsigned long)j; /* Different PRNG seed.. */
            if (j > 0)
            {
                vow.instance->process(); // get new initial_function_version
            }

            // run attack
            cycles1 = cpu_cycles();
            // printf("starting vow++ with vow.instance->PRNG_SEED = %lu\n", vow.instance->PRNG_SEED);
            vow.run();
            // printf ("vow run ended\n");
            cycles2 = cpu_cycles();
            cycles = cycles + (cycles2 - cycles1);

            // collect statistics
            success &= vow.success;
            random_functions += vow.final_avg_random_functions;
            collisions += vow.collisions;
            mem_collisions += vow.mem_collisions;
            dist_points += vow.dist_points;
            number_steps_collect += vow.number_steps_collect;
            number_steps_locate += vow.number_steps_locate;
            number_steps = number_steps_collect + number_steps_locate;
            dist_cols += vow.dist_cols.size;
            calendar_time += vow.wall_time;
            total_time += vow.total_time;
            // printf("\r%d", j);
        }

        if (collect_stats)
        {
            printf("\n");
            printf("Number of function iterations (i): \t\t%.2f\n", (double)number_steps / (double)random_functions);
            printf("\tFor collecting dist. points: \t\t%.2f (%.2f%%)\n",
                   ((double)number_steps_collect / (double)random_functions),
                   100 * ((double)number_steps_collect / (double)number_steps));
            printf("\tFor locating collisions: \t\t%.2f (%.2f%%)\n",
                   ((double)number_steps_locate / (double)random_functions),
                   100 * ((double)number_steps_locate / (double)number_steps));
            printf("Number of collisions per function: \t\t%.2f (expected 1.3w = %.2f, ratio = %.2f)\n", ((double)collisions / (double)random_functions),
                   1.3 * pow(2, (double)vow.instance->MEMORY_LOG_SIZE),
                   ((double)collisions / (double)random_functions) / (1.3 * pow(2, (double)vow.instance->MEMORY_LOG_SIZE)));
            printf("Number of distinct collisions per function (c): %.2f (expected 1.1w = %.2f, ratio = %.2f)\n",
                   ((double)dist_cols / (double)random_functions),
                   1.1 * pow(2, (double)vow.instance->MEMORY_LOG_SIZE),
                   ((double)dist_cols / (double)random_functions) / (1.1 * pow(2, (double)vow.instance->MEMORY_LOG_SIZE)));
            printf("\n");
            printf("Expected number of function versions (n/(2c)): \t%.2f (expected 0.45n/w = %.2f, ratio = %.2f)\n",
                   pow(2, (double)vow.instance->NBITS_STATE) / (2 * ((double)dist_cols / (double)random_functions)),
                   0.45 * pow(2, (double)vow.instance->NBITS_STATE) / pow(2, (double)vow.instance->MEMORY_LOG_SIZE),
                   (pow(2, (double)vow.instance->NBITS_STATE) / (2 * ((double)dist_cols / (double)random_functions))) /
                       (0.45 * pow(2, (double)vow.instance->NBITS_STATE) / pow(2, (double)vow.instance->MEMORY_LOG_SIZE)));
            printf("Expected total run-time (in/(2c)): \t\t%.2f (expected %cn^3/w = %.2f, ratio = %.2f)\n",
                   ((double)number_steps / (double)random_functions) * pow(2, vow.instance->NBITS_STATE) / (2 * ((double)dist_cols / (double)random_functions)),
                   251,
                   sqrt(pow(pow(2, vow.instance->NBITS_STATE), 3) / pow(2, vow.instance->MEMORY_LOG_SIZE)),
                   (((double)number_steps / (double)random_functions) * pow(2, vow.instance->NBITS_STATE) / (2 * ((double)dist_cols / (double)random_functions))) / sqrt(pow(pow(2, vow.instance->NBITS_STATE), 3) / pow(2, vow.instance->MEMORY_LOG_SIZE)));
        }
        else
        {
            printf("All tests succesful: \t\t\t\t%s\n", success ? "Yes" : "No");
            printf("\n");
            printf("Number of function iterations: \t\t\t%.2f (expected sqrt(n^3/w) = %.2f, ratio = %.2f)\n",
                   (double)number_steps / (double)BENCH_LOOPS,
                   sqrt(pow(pow(2, vow.instance->NBITS_STATE), 3) / pow(2, vow.instance->MEMORY_LOG_SIZE)),
                   ((double)number_steps / (double)BENCH_LOOPS) / (sqrt(pow(pow(2, vow.instance->NBITS_STATE), 3) / pow(2, vow.instance->MEMORY_LOG_SIZE))));
            printf("\tFor collecting dist. points: \t\t%.2f (%.2f%%)\n",
                   ((double)number_steps_collect / (double)BENCH_LOOPS),
                   100 * ((double)number_steps_collect / (double)number_steps));
            printf("\tFor locating collisions: \t\t%.2f (%.2f%%)\n",
                   ((double)number_steps_locate / (double)BENCH_LOOPS),
                   100 * ((double)number_steps_locate / (double)number_steps));
            printf("Number of function versions: \t\t\t%.2f (expected 0.45n/w = %.2f, ratio = %.2f)\n",
                   (double)random_functions / (double)BENCH_LOOPS,
                   0.45 * pow(2, vow.instance->NBITS_STATE) / pow(2, vow.instance->MEMORY_LOG_SIZE),
                   ((double)random_functions / (double)BENCH_LOOPS) /
                       (0.45 * pow(2, vow.instance->NBITS_STATE) / pow(2, vow.instance->MEMORY_LOG_SIZE)));
            printf("Number of collisions per function: \t\t%.2f (expected 1.3w = %.2f, ratio = %.2f)\n",
                   ((double)collisions / (double)random_functions),
                   1.3 * pow(2, vow.instance->MEMORY_LOG_SIZE),
                   (((double)collisions / (double)random_functions)) / (1.3 * pow(2, vow.instance->MEMORY_LOG_SIZE)));
        }

        printf("\nTotal time                        : %.2f", total_time);
        printf("\nTotal time    (avg per iteration) : %.2f", total_time / BENCH_LOOPS);
        printf("\nCalendar time                     : %.2f", calendar_time);
        printf("\nCalendar time (avg per iteration) : %.2f", calendar_time / BENCH_LOOPS);
        printf("\nCycle count   (avg per iteration) : %lld\n", cycles / BENCH_LOOPS);
    }

    for (int i = 0; i < NUM_INSTS_STATS; i++)
    {
        delete inst_stats[i];
    }

    return 0;
}

int main(int argc, char **argv)
{
    int Status = PASSED;
    uint64_t n_cores = 1;       // One core by default
    bool hansel_gretel = false; // Hansel&Gretel optimization is disabled by default
    uint64_t max_crumbs = 10;
    bool collect_stats = false; // Extra collection of stats is disabled by default
    bool help_flag = false;
    int MAX_ARGSplus1 = 6; // Current format: "test_vOW_gen -n [N_CORES] -H -s -h"

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

    Status = stats_vow(n_cores, hansel_gretel, collect_stats, max_crumbs); // Testing
    if (Status != PASSED)
    {
        printf("\n\n   Error detected while running attack... \n\n");
        return 1;
    }

    help:
    if (help_flag)
    {
        printf("\n Usage:");
        printf("\n test_vOW_gen -n [N_CORES] -H [MAX_CRUMBS] -s -h \n");
        printf("\n -n : number of cores (one core by default). Maximum 1000 cores.");
        printf("\n -H : Hansel&Gretel optimization on (off by default).");
        printf("\n -s : collection of attack stats on (off by default).");
        printf("\n -h : this help.\n\n");
    }

    #ifdef _WIN64
    system("PAUSE");
    #endif

    return Status;
}
