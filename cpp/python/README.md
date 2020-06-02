# Python scripts for running experiments

## `gen.py`

```
usage: gen.py [-h] [-run_full_atk] [-min_cpus MIN_CPUS] [-max_cpus MAX_CPUS]
              [-min_nbits MIN_NBITS] [-max_nbits MAX_NBITS] [-min_mem MIN_MEM]
              [-max_mem MAX_MEM] [-step STEP] [-iterations ITERATIONS]
              [-seed SEED] [-server SERVER] [-no_hag] [-crumbs CRUMBS]

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
  -no_hag               disable H&G
  -crumbs CRUMBS        number of H&G crumbs
```
