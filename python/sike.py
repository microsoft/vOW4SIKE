#!/usr/bin/env python2

import vow_sike.sike_vow as vow
from sike_vow_instances import load_instance, insts_stats
import pickle
import psutil
import json
import time
import random
import sys
from math import sqrt, ceil
from extract_stats import extract_stats

GLOBALS = {
    "ALPHA": 2.25,
    "BETA": 10.0,
    "GAMMA": 20.0,
    "MAX_THREADS": psutil.cpu_count(logical=False) # logical=True includes HT, logical=False excludes it
}


def memory(
    run_full_atk=True,
    delta = 0,
    max_crumbs = 10,
    min_cpus=1,
    max_cpus=2,
    min_mem=9,
    max_mem=13,
    modulus=None,
    step=1,
    iterations=0,
    prng_seed=0xdeadbeef,
    server=""
):

    # input sanitization
    if min_cpus > max_cpus or min_cpus <= 0:
        raise ValueError("Error: invalid (min_cpus, max_cpus)")

    if min_mem > max_mem or min_mem <= 0:
        raise ValueError("Error: invalid (min_mem, max_mem)")

    if prng_seed == 0:
        prng_seed = 0xdeadbeef
    random.seed(prng_seed)

    hansel_and_gretel = bool(max_crumbs > 0)

    print "max_cpus", max_cpus
    if iterations == 0:
        print "iterations =", "ceil(2**16 / 2**memory_log_size)"
    else:
        print "iterations =", iterations

    print "min_mem", min_mem
    print "max_mem", max_mem

    f = open('%ssike_full_atk_%s_hag_%s_precomp_%s'%(server, run_full_atk, hansel_and_gretel, delta), 'a')

    insts_stats = load_instance(modulus)

    for inst in insts_stats:
        print "e", inst.e
        nbits_state = inst.e - 1
        for memory_log_size in range(min_mem, max_mem+1, step):
            inst.MEMORY_LOG_SIZE = memory_log_size
            inst.PRNG_SEED = 0
            inst.N_OF_CORES = 0
            inst.HANSEL_GRETEL = hansel_and_gretel
            inst.MAX_CRUMBS = max_crumbs
            inst.delta = delta

            S = vow.shared_state_t()
            vow.init_shared_state(inst, S)

            for n_threads in range(min_cpus, min(GLOBALS["MAX_THREADS"], max_cpus)+1, step):
                print "modulus %s e %s memory_log_size %s delta %s threads %s h&g %s h&g max_crumbs %s full attack %s"%(
                    modulus,
                    inst.e,
                    memory_log_size,
                    delta,
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
                    "full_data": [],
                    "expected": {}
                }

                inst.N_OF_CORES = n_threads
                S.N_OF_CORES = n_threads

                # Average stats over runs depending on memory size
                if iterations == 0:
                    iterations = int(ceil(2. ** (16 - memory_log_size)))

                # the way the threads pick seeds is sequential,
                # so while we fix that we really want here
                # to hash the seed itself between experiments

                if run_full_atk:
                    # test until solution
                    S.MAX_FUNCTION_VERSIONS = (2**32 // iterations)
                    S.collect_vow_stats = False
                    if (100*S.MAX_FUNCTION_VERSIONS < (0.45*3*2**nbits_state / 2**memory_log_size)):
                        raise ValueError("Too many iterations to fit into range of 2**32")
                else:
                    # tests only one random function version
                    S.MAX_FUNCTION_VERSIONS = 1
                    S.collect_vow_stats = True

                base_prng_seed = random.randint(1, 2**64 - 1)
                S.initial_function_version = random.randint(0, 2**32 - 1)

                for iteration in range(1,iterations+1):
                    vow.reset_shared_state(S)

                    S.PRNG_SEED = (base_prng_seed + iteration) % 2**64
                    if S.PRNG_SEED == 0:
                        S.PRNG_SEED = 1
                    S.initial_function_version = (S.initial_function_version + S.MAX_FUNCTION_VERSIONS) % 2**32

                    # run
                    vow.vOW(S)

                    # save stats
                    experiment["total_number"] += 1
                    experiment["success"] += int(S.success)
                    experiment["wall_time"] += S.wall_time
                    experiment["total_time"] += S.total_time
                    experiment["cycles"] += S.cpu_cycles
                    experiment["collisions"] += S.collisions
                    experiment["mem_collisions"] += S.mem_collisions
                    experiment["dist_points"] += S.dist_points
                    experiment["dist_cols"] += S.dist_cols.size
                    experiment["num_steps_collect"] += S.number_steps_collect
                    experiment["num_steps_locate"] += S.number_steps_locate
                    experiment["num_steps"] += S.number_steps
                    experiment["avg_random_functions"] += S.final_avg_random_functions
                    experiment["full_data"].append({
                        "success": int(S.success),
                        "wall_time": S.wall_time,
                        "total_time": S.total_time,
                        "cycles": S.cpu_cycles,
                        "collisions": S.collisions,
                        "mem_collisions": S.mem_collisions,
                        "dist_points": S.dist_points,
                        "dist_cols": S.dist_cols.size,
                        "num_steps_collect": S.number_steps_collect,
                        "num_steps_locate": S.number_steps_locate,
                        "num_steps": S.number_steps,
                        "avg_random_functions": S.final_avg_random_functions,
                    })
                    if not run_full_atk:
                        experiment["full_data"][-1]["coll_per_fun"] = S.collisions / S.final_avg_random_functions
                        experiment["full_data"][-1]["dist_cols_per_fun"] = S.dist_cols.size / S.final_avg_random_functions

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

                json_out = json.dumps({
                    'k': (modulus, memory_log_size, n_threads), 
                    'v': experiment
                })
                print >> f, json_out
                print json_out
                f.flush()
                sys.stdout.flush()
            vow.free_shared_state(S)


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-run_full_atk", action='store_true', help="run full attack (off by default, hence running only one function version).")
    parser.add_argument("-min_cpus", type=int, default=1, help="min number of threads (default 1)")
    parser.add_argument("-max_cpus", type=int, default=2, help="max number of threads (default 2)")
    parser.add_argument("-m", type=str, default="p_32_20", help="modulus (%s)" % ", ".join(insts_stats.keys()))
    parser.add_argument("-min_mem", type=int, default=9, help="min mem_bits (default 9)")
    parser.add_argument("-max_mem", type=int, default=13, help="max mem_bits (default 13)")
    parser.add_argument("-step", type=int, default=1, help="step value for all three counters (default 1)")
    parser.add_argument("-iterations", type=int, default=0, help="fixed number of tests for every param set (default 2^16/w)")
    parser.add_argument("-seed", type=int, default=0xdeadbeef, help="number of tests for every param set (default 0xdeadbeef)")
    parser.add_argument("-server", type=str, default="", help="server name (eg solardiesel_)")
    parser.add_argument("-pd", type=int, default=0, help="depth of precomputation (default 0)")
    parser.add_argument("-no_hag", action="store_true", help="disable H&G")
    parser.add_argument("-crumbs", type=int, default=10, help="number of crumbs (default 10)")
    args = parser.parse_args()

    if args.no_hag:
        try:
            print "classic vOW"
            memory(
                run_full_atk=args.run_full_atk,
                delta=args.pd,
                max_crumbs=0,
                min_cpus=args.min_cpus,
                max_cpus=args.max_cpus,
                min_mem=args.min_mem,
                max_mem=args.max_mem,
                modulus=args.m,
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
                delta=args.pd,
                max_crumbs=args.crumbs,
                min_cpus=args.min_cpus,
                max_cpus=args.max_cpus,
                min_mem=args.min_mem,
                max_mem=args.max_mem,
                modulus=args.m,
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
