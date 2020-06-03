#!/usr/bin/env python

import pyvow.vow as vow
import pickle
import psutil
import json
import time
import random
import hashlib
from math import sqrt, ceil
from extract_stats import extract_stats

GLOBALS = {
    "ALPHA": 2.25,
    "BETA": 10.0,
    "GAMMA": 20.0,
    "MAX_THREADS": psutil.cpu_count(logical=False), # logical=True includes HT, logical=False excludes it
}


def memory(
    run_full_atk=False,
    max_crumbs=10,
    min_cpus=1,
    max_cpus=2,
    min_nbits=14,
    max_nbits=16,
    min_mem=9,
    max_mem=13,
    step=1,
    iterations=0,
    prng_seed=0xdeadbeef,
    server=""
):

    # input sanitization
    if min_cpus > max_cpus or min_cpus <= 0:
        raise ValueError("Error: invalid (min_cpus, max_cpus)")

    if min_nbits > max_nbits or min_nbits <= 0:
        raise ValueError("Error: invalid (min_nbits, max_nbits)")
    
    if min_mem > max_mem or min_mem <= 0:
        raise ValueError("Error: invalid (min_mem, max_mem)")

    if prng_seed == 0:
        prng_seed = 0xdeadbeef
    random.seed(prng_seed)
    salt = 0

    hansel_and_gretel = bool(max_crumbs > 0)

    print "max_cpus =", max_cpus
    print "min_nbits =", min_nbits
    print "max_nbits =", max_nbits
    if iterations == 0:
        print "iterations =", "ceil(2**16 / 2**memory_log_size)"
    else:
        print "iterations =", iterations

    print "max_mem", max_mem
    print "min_mem", min_mem

    # stats collector
    stats = vow.statistics_t()

    # stats = {}
    f = open('%sgen_full_atk_%s_hag_%s'%(server, run_full_atk, hansel_and_gretel), 'a')

    for nbits_state in range(min_nbits, max_nbits+1, step):
        for memory_log_size in range(min_mem, max_mem+1, step):
            t = vow.instance_t(64,
                nbits_state,
                memory_log_size,
                GLOBALS["ALPHA"],
                GLOBALS["BETA"],
                GLOBALS["GAMMA"],
                0, # PRNG seed
            )
            vow.set_n_cores(t, 0)
            vow.set_hansel_gretel(t, hansel_and_gretel, max_crumbs)

            S = vow.new_vow(t)

            for n_threads in range(min_cpus, min(GLOBALS["MAX_THREADS"], max_cpus)+1, step):
                print "nbits_state %s memory_log_size %s threads %s h&g %s h&g max_crumbs %s full attack %s"%(
                    nbits_state,
                    memory_log_size,
                    n_threads,
                    hansel_and_gretel,
                    max_crumbs,
                    run_full_atk
                )
                experiment = {
                    "total_number": 0,
                    "success": 0.,
                    "wall_time": 0.,
                    "total_time": 0.,
                    "cycles": 0,
                    "collisions": 0.,
                    "mem_collisions": 0.,
                    "dist_points": 0.,
                    "dist_cols": 0.,
                    "num_steps_collect": 0.,
                    "num_steps_locate": 0.,
                    "num_steps": 0.,
                    "avg_random_functions": 0.,
                    "run_full_atk": run_full_atk,
                    "hansel_and_gretel": hansel_and_gretel,
                    "max_crumbs": max_crumbs,
                    "full_data": [],
                    "expected": {}
                }

                # Average stats over runs depending on memory size
                if iterations == 0:
                    iterations = int(ceil(2. ** (16 - memory_log_size)))


                vow.set_n_cores(t, n_threads)
                vow.set_n_cores(S, n_threads)

                if run_full_atk:
                    # test until solution
                    max_function_versions = 2**32 // iterations
                    collect_vow_stats = False
                    if (100*max_function_versions < (0.45*3*2**nbits_state / 2**memory_log_size)):
                        raise ValueError("Too many iterations to fit into range of 2**32")
                else:
                    # tests only one random function version
                    max_function_versions = 1
                    collect_vow_stats = True

                vow.set_max_function_versions(S, max_function_versions)
                vow.set_collect_vow_stats(S, collect_vow_stats)

                for iteration in range(1,iterations+1):
                    vow.reset_vow(S)

                    # the way the threads pick seeds is sequential,
                    # so while we fix that we really want here
                    # to hash the seed itself between experiments
                    salt += 1
                    derived_prng_seed = int(hashlib.sha256(str(prng_seed + salt)).hexdigest(), 16) % (2**64)
                    vow.set_prng_seed(S, derived_prng_seed)
                    vow.process_instance(S)

                    # run
                    vow.run_vow(S)

                    # collect statistics
                    vow.read_stats_vow(S, stats)

                    # save stats
                    experiment["total_number"] += 1
                    experiment["success"] += int(stats.success)
                    experiment["wall_time"] += stats.calendar_time
                    experiment["total_time"] += stats.total_time
                    experiment["cycles"] += stats.cycles
                    experiment["collisions"] += stats.collisions
                    experiment["mem_collisions"] += stats.mem_collisions
                    experiment["dist_points"] += stats.dist_points
                    experiment["dist_cols"] += stats.dist_cols
                    experiment["num_steps_collect"] += stats.number_steps_collect
                    experiment["num_steps_locate"] += stats.number_steps_locate
                    experiment["num_steps"] = experiment["num_steps_collect"] + experiment["num_steps_locate"]
                    experiment["avg_random_functions"] += stats.random_functions

                    experiment["full_data"].append({
                        "prng_seed": prng_seed,
                        "salt": salt,
                        "derived_prng_seed": derived_prng_seed,
                        "success": int(stats.success),
                        "wall_time": stats.calendar_time,
                        "total_time": stats.total_time,
                        "cycles": stats.cycles,
                        "collisions": stats.collisions,
                        "mem_collisions": stats.mem_collisions,
                        "dist_points": stats.dist_points,
                        "dist_cols": stats.dist_cols,
                        "num_steps_collect": stats.number_steps_collect,
                        "num_steps_locate": stats.number_steps_locate,
                        "num_steps": stats.number_steps_collect + stats.number_steps_locate,
                        "avg_random_functions": stats.random_functions,
                    })
                    if not run_full_atk:
                        experiment["full_data"][-1]["coll_per_fun"] = stats.collisions / stats.random_functions
                        experiment["full_data"][-1]["dist_cols_per_fun"] = stats.dist_cols / stats.random_functions

                # average results
                denominator = float(experiment["total_number"])
                experiment["success"] /= denominator
                experiment["wall_time"] /= denominator
                experiment["total_time"] /= denominator
                experiment["cycles"] /= denominator
                experiment["collisions"] /= denominator
                experiment["mem_collisions"] /= denominator
                experiment["dist_points"] /= denominator
                experiment["dist_cols"] /= denominator
                experiment["num_steps_collect"] /= denominator
                experiment["num_steps_locate"] /= denominator
                experiment["num_steps"] /= denominator
                experiment["avg_random_functions"] /= denominator

                extract_stats(experiment, {
                    "memory_log_size": memory_log_size,
                    "nbits_state": nbits_state,
                    "n_threads": n_threads,
                    "alpha": GLOBALS["ALPHA"],
                    "run_full_atk": run_full_atk
                })

                print >> f, json.dumps({
                    'k': (nbits_state, memory_log_size, n_threads), 
                    'v': experiment
                })
            vow.delete_vow(S)


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-run_full_atk", action='store_true', help="run full attack (off by default, hence running only one function version).")
    parser.add_argument("-min_cpus", type=int, default=1, help="min number of threads (default 1)")
    parser.add_argument("-max_cpus", type=int, default=2, help="max number of threads (default 2)")
    parser.add_argument("-min_nbits", type=int, default=14, help="min bitlength of state (default 14)")
    parser.add_argument("-max_nbits", type=int, default=16, help="max bitlength of state (default 16)")
    parser.add_argument("-min_mem", type=int, default=9, help="min mem_bits (default 9)")
    parser.add_argument("-max_mem", type=int, default=13, help="max mem_bits (default 13)")
    parser.add_argument("-step", type=int, default=1, help="step value for all three counters (default 1)")
    parser.add_argument("-iterations", type=int, default=0, help="fixed number of tests for every param set (default 2^16/w)")
    parser.add_argument("-seed", type=int, default=0xdeadbeef, help="experiment prng seed, for reproducibility (default 0xdeadbeef)")
    parser.add_argument("-server", type=str, default="", help="server name (eg solardiesel_)")
    parser.add_argument("-no_hag", action="store_true", help="disable H&G")
    parser.add_argument("-crumbs", type=int, default=0, help="number of H&G crumbs")
    args = parser.parse_args()

    print "running full attack:", args.run_full_atk, "\n"

    if args.crumbs == 0:
        try:
            print "classic vOW"
            memory(
                run_full_atk=args.run_full_atk,
                max_crumbs=0,
                min_cpus=args.min_cpus,
                max_cpus=args.max_cpus,
                min_nbits=args.min_nbits,
                max_nbits=args.max_nbits,
                min_mem=args.min_mem,
                max_mem=args.max_mem,
                step=args.step,
                iterations=args.iterations,
                prng_seed=args.seed,
                server=args.server
            )
        except ValueError as e:
            print e
            print "Passing to next attack"
            print

        if not args.no_hag:
            try:
                print "h&g"
                memory(
                    run_full_atk=args.run_full_atk,
                    max_crumbs=10,
                    min_cpus=args.min_cpus,
                    max_cpus=args.max_cpus,
                    min_nbits=args.min_nbits,
                    max_nbits=args.max_nbits,
                    min_mem=args.min_mem,
                    max_mem=args.max_mem,
                    step=args.step,
                    iterations=args.iterations,
                    prng_seed=args.seed,
                    server=args.server
                )
            except ValueError as e:
                print e
                print "Passing to next attack"
                print
    else:
        try:
            print "h&g"
            memory(
                run_full_atk=args.run_full_atk,
                max_crumbs=args.crumbs,
                min_cpus=args.min_cpus,
                max_cpus=args.max_cpus,
                min_nbits=args.min_nbits,
                max_nbits=args.max_nbits,
                min_mem=args.min_mem,
                max_mem=args.max_mem,
                step=args.step,
                iterations=args.iterations,
                prng_seed=args.seed,
                server=args.server
            )
        except ValueError as e:
            print e
            print "Passing to next attack"
            print


import __main__
if hasattr(__main__, "__file__") and __name__ == "__main__":
    main()
