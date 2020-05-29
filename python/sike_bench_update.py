#!/usr/bin/env python2

import vow_sike.sike_vow as vow
from sike_vow_instances import load_instance, insts_stats
import pickle
import psutil
import json
import time
import random
from math import sqrt, ceil
from extract_stats import extract_stats

GLOBALS = {
    "ALPHA": 2.25,
    "BETA": 10.0,
    "GAMMA": 20.0,
    "MAX_THREADS": psutil.cpu_count(logical=False) # logical=True includes HT, logical=False excludes it
}


def bench(
    modulus=None,
    min_delta = 0,
    max_delta = 1,
    step=2,
    iterations=100,
    prng_seed=0xdeadbeef,
    server=""
):

    if prng_seed == 0:
        prng_seed = 0xdeadbeef
    random.seed(prng_seed)

    f = open('%ssike_bench_Update_%s'%(server, modulus), 'a')

    insts_stats = load_instance(modulus)

    for inst in insts_stats:
        nbits_state = inst.e - 1
        inst.PRNG_SEED = 0
        inst.delta = min_delta

        S = vow.shared_state_t()
        P = vow.private_state_t()

        for delta in range(min_delta, min(max_delta+1, inst.e-1), step):
            print "modulus %s e %s delta %s "%(
                modulus,
                inst.e,
                delta
            )
            experiment = {
                "total_number": 0,
                "wall_time": 0.,
                "total_time": 0.,
                "cycles": 0,
                "precomp_time": 0.
            }
            inst.delta = delta

            experiment["precomp_time"] = -time.clock()
            vow.init_shared_state(inst, S)
            experiment["precomp_time"] += time.clock()
            vow.init_private_state(S, P)
            S.HANSEL_GRETEL = False

            experiment["total_time"] = -time.clock()

            for iteration in range(1,iterations+1):
                S.wall_time = -time.clock()
                S.cpu_cycles = -vow.cpu_cycles()

                # run
                vow.UpdateSIDH(P)

                S.cpu_cycles += vow.cpu_cycles()
                S.wall_time += time.clock()

                # save stats
                experiment["total_number"] += 1
                experiment["wall_time"] += S.wall_time
                experiment["cycles"] += S.cpu_cycles

            experiment["total_time"] += time.clock()

            # average results
            denominator = float(experiment["total_number"])
            experiment["wall_time"] /= denominator
            experiment["cycles"] /= denominator

            print >> f, json.dumps({
                'k': (modulus, delta), 
                'v': experiment
            })
            vow.free_private_state(P)
            vow.free_shared_state(S)


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-m", type=str, default="p_32_20", help="modulus (%s)" % ", ".join(insts_stats.keys()))
    parser.add_argument("-step", type=int, default=2, help="step value for all three counters (default 2)")
    parser.add_argument("-iterations", type=int, default=1, help="fixed number of iterations for benchmarking")
    parser.add_argument("-seed", type=int, default=0xdeadbeef, help="number of tests for every param set (default 0xdeadbeef)")
    parser.add_argument("-server", type=str, default="", help="server name (eg solardiesel_)")
    parser.add_argument("-pd", type=int, default=0, help="depth of precomputation (default 0)")
    parser.add_argument("-min_delta", type=int, default=0, help="min delta (default 0)")
    parser.add_argument("-max_delta", type=int, default=100, help="max delta (default 100)")
    args = parser.parse_args()

    try:
        print "benchmark UpdateSt"
        bench(
            modulus=args.m,
            min_delta=args.min_delta,
            max_delta=args.max_delta,
            step=args.step,
            iterations=args.iterations,
            prng_seed=args.seed,
            server=args.server
        )
    except ValueError as e:
        print e
        print "Error benchmarking UpdateSt"
        print


import __main__
if hasattr(__main__, "__file__") and __name__ == "__main__":
    main()
