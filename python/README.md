# Python scripts for running experiments

## `gen.py`

```
usage: gen.py [-h] [-run_full_atk] [-cpus CPUS] [-min_nbits MIN_NBITS]
              [-max_nbits MAX_NBITS] [-min_mem MIN_MEM] [-max_mem MAX_MEM]
              [-step STEP] [-iterations ITERATIONS] [-seed SEED]
              [-server SERVER]

optional arguments:
  -h, --help            show this help message and exit
  -run_full_atk         run full attack (off by default, hence running only
                        one function version).
  -min_cpus MIN_CPUS    min number of threads (default 1)
  -max_cpus MAX_CPUS    max number of threads (default 2)
  -min_nbits MIN_NBITS  min bitlength of state (default 14)
  -max_nbits MAX_NBITS  max bitlength of state (default 16)
  -min_mem MIN_MEM      min mem_bits (default 9)
  -max_mem MAX_MEM      max mem_bits (default 13)

  -step STEP            step value for all three counters (default 1)
  -iterations ITERATIONS
                        fixed number of tests for every param set (default
                        2^16/w)
  -seed SEED            experiment prng seed, for reproducibility (default
                        0xdeadbeef)
  -server SERVER        server name (eg solardiesel_)
```

For example, running `python gen.py -run_full_atk -max_cpus 20 -max_nbits 40 -step 2 -iterations 1000 -seed 1337 -server solardiesel_`
will run experiments for

- `nbits` in `[14, 16, 18, 20, ..., 38, 40]` (due to the `-step 2` parameter),
- using `mem_bits` in `[10, 12, 14, 16, 18, 20]` (due to the `-step 2` parameter + using up to `sqrt(nbits)` memory),
- using `[1, 3, 5, ... 19]` cpus (again due to the `-step 2` parameter),
- every experiment will be run 1000 times with different `PRNG_SEED` values (generating starting from the original `-seed 1337`),
- every experiment will run until a solution is found,
- every experiment will be run both without and with H&G,
- the output files will be called `solardiesel_full_atk_True_hag_False` and `solardiesel_full_atk_True_hag_True`.

## `sidh.py`

```
usage: sidh.py [-h] [-run_full_atk] [-cpus CPUS] [-m M] [-min_mem MIN_MEM]
               [-max_mem MAX_MEM] [-step STEP] [-iterations ITERATIONS]
               [-seed SEED] [-server SERVER] [-np]

optional arguments:
  -h, --help            show this help message and exit
  -run_full_atk         run full attack (off by default, hence running only
                        one function version).
  -min_cpus MIN_CPUS    min number of threads (default 1)
  -max_cpus MAX_CPUS    max number of threads (default 2)
  -m M                  modulus (p_32_20, p_48_30, p_56_35, p_52_33, p_44_27,
                        p_40_25, p_36_22, p_60_38)
  -min_mem MIN_MEM      max mem_bits (default 9)
  -max_mem MAX_MEM      max mem_bits (default 13)
  -step STEP            step value for all three counters (default 1)
  -iterations ITERATIONS
                        fixed number of tests for every param set (default
                        2^16/w)
  -seed SEED            experiment prng seed, for reproducibility (default
                        0xdeadbeef)
  -server SERVER        server name (eg solardiesel_)
  -np                   disable precomputation (by default precomputation
                        enabled)
```

- The `-run_full_atk`, `-max_cpus`, `-step`, `-iterations`, `-seed` and `-server` parameters are as for `gen.py`,
- `-m` is used to select a prime (in alternative to passing the value to `nbits_state` in `gen.py`),
- `-max_mem` is used to set a maximum amount of memory to use,
- passing `-np` disables precomputation,
- the output files are named similarly to the `gen.py` ones.

## `sike.py`

```
usage: sike.py [-h] [-run_full_atk] [-cpus CPUS] [-m M] [-min_mem MIN_MEM]
               [-max_mem MAX_MEM] [-step STEP] [-iterations ITERATIONS]
               [-seed SEED] [-server SERVER] [-pd PD]

optional arguments:
  -h, --help            show this help message and exit
  -run_full_atk         run full attack (off by default, hence running only
                        one function version).
  -min_cpus MIN_CPUS    min number of threads (default 1)
  -max_cpus MAX_CPUS    max number of threads (default 2)
  -m M                  modulus (p_32_20, p_48_30, p_56_35, p_52_33, p_44_27,
                        p_40_25, p_36_22, p_60_38)
  -min_mem MIN_MEM      min mem_bits (default 9)
  -max_mem MAX_MEM      max mem_bits (default 13)
  -step STEP            step value for all three counters (default 1)
  -iterations ITERATIONS
                        fixed number of tests for every param set (default
                        2^16/w)
  -seed SEED            number of tests for every param set (default
                        0xdeadbeef)
  -server SERVER        server name (eg solardiesel_)
  -pd PD                depth of precomputation (default 0)
```

- Parameters are as for `sidh.py`, except for the `-pd` parameter.
- Setting `-pd 0` gives no precomputation, else precomputation of depth `PD`.
- The output files will be called `servername_full_atk_ ... _precomputation_PD`.
