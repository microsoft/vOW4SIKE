#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import vow_sidh.sidh_vow as sidh_vow


def stats_vow(n_cores, precomp, max_crumbs, collect_stats, modulus='p_32_20'):

    # imports
    from math import sqrt

    # defines
    BENCH_LOOPS = 10

    # includes
    from sidh_vow_instances import load_instance
    insts_stats = load_instance(modulus)
    NUM_INSTS_STATS = len(insts_stats)

    # H & G
    hansel_gretel = bool(max_crumbs > 0)

    print "\nRunning vOW attack on SIDH",
    print "\n----------------------------------------------------------------------------------------\n"

    for i in range(NUM_INSTS_STATS):
        success = True
        random_functions = 0
        collisions = 0
        mem_collisions = 0
        dist_points = 0
        number_steps_collect = 0
        number_steps_locate = 0
        number_steps = 0
        dist_cols = 0
        calendar_time = 0.
        total_time = 0.
        cycles = 0

        insts_stats[i].N_OF_CORES = n_cores # Setting number of cores
        insts_stats[i].PRECOMP = precomp # Setting precomputation
        insts_stats[i].HANSEL_GRETEL = hansel_gretel # Setting Hansel&Gretel optimization
        insts_stats[i].MAX_CRUMBS = max_crumbs

        S = sidh_vow.shared_state_t()
        sidh_vow.init_shared_state(insts_stats[i], S)

        print "\n----------------------------------------------------------------------------------------\n"
        print "Instance:\t",
        print "n = %ld\t    " % (insts_stats[i].NBITS_K+1),
        print "w = %ld\t" % insts_stats[i].MEMORY_LOG_SIZE,
        print "α = %.2f\t" % insts_stats[i].ALPHA,
        print "β = %.2f\t" % insts_stats[i].BETA,
        print "γ = %.2f\t" % insts_stats[i].GAMMA,
        print "Modulus = %s" % insts_stats[i].MODULUS,
        print "\n"
        print "Number of iterations averaged over: \t\t%s" % BENCH_LOOPS
        print "Memory:  \t\t\t\t\tRAM"
        print "Number of cores: \t\t\t\t%s" % n_cores
        print "Hansel & Gretel: \t\t\t\t%s" % ("Yes" if hansel_gretel else "No")
        if hansel_gretel:
            print "Number of Hansel & Gretel crumbs: \t\t%s" % max_crumbs 
        print "Statistics only: \t\t\t\t%s" % ("Yes (only running one function version)" if collect_stats else "No")

        S.MAX_FUNCTION_VERSIONS = 100000
        S.collect_vow_stats = collect_stats
        for j in range(BENCH_LOOPS):
            sidh_vow.reset_shared_state(S)

            if j > 0:
                S.initial_function_version += 1 # Maintain across examples, but change
            S.PRNG_SEED += j # Different PRNG seed.., not exactly same as in c test code

            sidh_vow.vOW(S)

            success &= S.success
            random_functions += S.final_avg_random_functions
            collisions += S.collisions
            mem_collisions += S.mem_collisions
            dist_points += S.dist_points
            number_steps_collect += S.number_steps_collect
            number_steps_locate += S.number_steps_locate
            number_steps = number_steps_collect + number_steps_locate
            dist_cols += S.dist_cols.size
            calendar_time += S.wall_time
            total_time += S.total_time
            cycles += S.cpu_cycles

        sidh_vow.free_shared_state(S)

        if collect_stats:
            print ""
            print "Number of function iterations (i): \t\t%.2f" % (1. * number_steps / random_functions)
            print "\tFor collecting dist. points: \t\t%.2f" % (1. * number_steps_collect / random_functions),
            print "(%.2f%%)" % (100*(1.00 * number_steps_collect / number_steps))
            print "\tFor locating collisions: \t\t%.2f" % (1. * number_steps_locate / random_functions),
            print "(%.2f%%)" % (100*(1. * number_steps_locate / number_steps))
            print "Number of collisions per function: \t\t%.2f" % (1. * collisions / random_functions),
            print "(expected 1.3w = %.2f," % (1.3 * 2 ** insts_stats[i].MEMORY_LOG_SIZE),
            print "ratio = %.2f)" % ((1. * collisions / random_functions) / (1.3 * 2 ** insts_stats[i].MEMORY_LOG_SIZE))
            print "Number of distinct collisions per function (c): %.2f" % (1. * dist_cols / random_functions),
            print "(expected 1.1w = %.2f," % (1.1 * 2 ** insts_stats[i].MEMORY_LOG_SIZE),
            print "ratio = %.2f)" % ((1. * dist_cols / random_functions) / (1.1 * 2 ** insts_stats[i].MEMORY_LOG_SIZE))
            print ""
            print "Expected number of function versions (n/(2c)): \t%.2f" % (1. * 3 * 2 ** (insts_stats[i].NBITS_K+1) / (2. * dist_cols / random_functions)),
            print "(expected 0.45n/w = %.2f," % (0.45 * 3 * 2 ** (insts_stats[i].NBITS_K+1) / 2 ** insts_stats[i].MEMORY_LOG_SIZE),
            print "ratio = %.2f)" % ((1. * 3 * 2 ** (insts_stats[i].NBITS_K+1) / (2. * dist_cols / random_functions)) / (0.45 * 3 * 2 ** (insts_stats[i].NBITS_K+1) / 2 ** insts_stats[i].MEMORY_LOG_SIZE))
            print "Expected total run-time (in/(2c)): \t\t%.2f" % ((1. * number_steps / random_functions) * 3 * 2 ** (insts_stats[i].NBITS_K+1) / ( 2. * dist_cols / random_functions )),
            print "(expected √(n^3/w) = %.2f," % (sqrt( ( (3 * 2 ** (insts_stats[i].NBITS_K+1)) ** 3 ) / (2 ** insts_stats[i].MEMORY_LOG_SIZE) )),
            print "ratio = %.2f)" % (((1. * number_steps / random_functions) * 3 * 2 ** (insts_stats[i].NBITS_K+1) / ( 2. * dist_cols / random_functions )) / sqrt( ( (3 * 2 ** (insts_stats[i].NBITS_K+1)) ** 3 ) / (2 ** insts_stats[i].MEMORY_LOG_SIZE) ))
        else:
            print "All tests succesful: \t\t\t\t%s" % ("Yes" if success else "No")
            print ""
            print "Number of function iterations: \t\t\t%.2f" % (1. * number_steps / BENCH_LOOPS),
            print "(expected √n^3/w = %.2f," % (sqrt(((3 * 2**((insts_stats[i].NBITS_K+1)))**3) / (2**(insts_stats[i].MEMORY_LOG_SIZE)))),
            print "ratio = %.2f)" % ((1. * number_steps / BENCH_LOOPS) / (sqrt(((3 * 2**((insts_stats[i].NBITS_K+1)))**3) / (2**(insts_stats[i].MEMORY_LOG_SIZE)))))
            print "\tFor collection dist. points: \t\t%.2f" % (1. * number_steps_collect / BENCH_LOOPS),
            print "(%.2f%%)" % (100 * number_steps_collect / number_steps)
            print "\tFor locating collisions: \t\t%.2f" % (1. * number_steps_locate / BENCH_LOOPS),
            print "(%.2f%%)" % (100 * number_steps_locate / number_steps)
            print "Number of function versions: \t\t\t%.2f" % (1. * random_functions / BENCH_LOOPS),
            print "(expected 0.45n/w = %.2f" % (0.45 * 3 * 2**(insts_stats[i].NBITS_K+1) / 2**insts_stats[i].MEMORY_LOG_SIZE),
            print "ratio = %.2f)" % ((1. * random_functions / BENCH_LOOPS) / (0.45 * 3 * 2**(insts_stats[i].NBITS_K+1) / 2**insts_stats[i].MEMORY_LOG_SIZE))
            print "Number of collisions per function: \t\t%.2f" % (1. * collisions / random_functions),
            print "(expected 1.3 = %.2f" % (1.3 * 2**insts_stats[i].MEMORY_LOG_SIZE),
            print "ratio = %.2f)" % ((1. * collisions / random_functions) / (1.3 * 2**insts_stats[i].MEMORY_LOG_SIZE))

        print "\nTotal time                        : %.2f" % (total_time),
        print "\nTotal time    (avg per iteration) : %.2f" % (total_time/BENCH_LOOPS),
        print "\nCalendar time                     : %.2f" % (calendar_time),
        print "\nCalendar time (avg per iteration) : %.2f" % (calendar_time/BENCH_LOOPS),
        print "\nCycle count   (avg per iteration) : %s\n\n" % (cycles/BENCH_LOOPS),

    return 0


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", type=int, default=1, help="number of cores (one core by default)")
    parser.add_argument("-H", type=int, default=0, help="Hansel&Gretel max crumbs (off by default).")
    parser.add_argument("-s", action='store_true', help="collection of attack stats on (off by default).")
    parser.add_argument("-p", action='store_true', help="use precomputation (off by default).")
    args = parser.parse_args()

    status = stats_vow(args.n, args.p, args.H, args.s)
    if status:
        print "\n\n   Error detected while running attack... \n\n",
        return 1


import __main__
if hasattr(__main__, "__file__") and __name__ == "__main__":
    main()
