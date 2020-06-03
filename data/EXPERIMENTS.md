# Experiment list

For a generic XOF, we run experiments on an AES-NI-based and an xxHash-based one.
To switch between the two, change settings in `/src/prng.h` and recompile.

The cited output files can be found in `/data/raw_data/`.

## Reproducing the results by Adj et al. [5]

### Table 2

For reproducing Table 2, the code calls one function version (1 fv) and averages over 20 instances for the following parameters:

```
nbits_state 20, 24, 28, 32, 36
mem_bits 2, 3, .. 18
```

Done (both xxhash and aes-ni xof):

```
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_2_Adj_et_al_ -no_hag
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 24 -max_nbits 24 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_2_Adj_et_al_ -no_hag
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 28 -max_nbits 28 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_2_Adj_et_al_ -no_hag
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 32 -max_nbits 32 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_2_Adj_et_al_ -no_hag
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 36 -max_nbits 36 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_2_Adj_et_al_ -no_hag
```

Output files:

```
solardiesel_reproducing_table_2_Adj_et_al_gen_full_atk_False_hag_False
atomkohle_aes_xof_reproducing_table_2_Adj_et_al_gen_full_atk_False_hag_False
```

### Table 3

To reproduce Table 3, the code calls the full attack over a set of curves with `e_A in range(32, 45, 2)`. Runs are averaged over 25 seeds.

We can run the experiments for `e_A in range(32, 45, 4)` where `e_A = 0 mod 4`. We do maximal precomputation for SIDH & SIKE to increase the speed of the computation. The SIDH numbers should match Adj et al.'s [5] (we do better on *average* than they did in their table, and equal for the *median*), while SIKE should do better due to the decreased set size. We use 20 cores as they did.

Run of 1000 `sidh` iterations *with precomputation* run on solardiesel:

```
python2 sidh.py -run_full_atk -min_cpus 20 -max_cpus 20 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_32_20_ -no_hag
python2 sidh.py -run_full_atk -min_cpus 20 -max_cpus 20 -m p_36_22 -min_mem 10 -max_mem 10 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_36_22_ -no_hag
python2 sidh.py -run_full_atk -min_cpus 20 -max_cpus 20 -m p_40_25 -min_mem 11 -max_mem 11 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_40_25_ -no_hag
python2 sidh.py -run_full_atk -min_cpus 20 -max_cpus 20 -m p_44_27 -min_mem 13 -max_mem 13 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_44_27_ -no_hag
```

Output files:

```
solardiesel_reproducing_table_3_adj_p_32_20_sidh_full_atk_True_hag_False_precomp_True
solardiesel_reproducing_table_3_adj_p_36_22_sidh_full_atk_True_hag_False_precomp_True
solardiesel_reproducing_table_3_adj_p_40_25_sidh_full_atk_True_hag_False_precomp_True
solardiesel_reproducing_table_3_adj_p_44_27_sidh_full_atk_True_hag_False_precomp_True
```

Run of 1000 `sike` iterations *with precomputation* run on solardiesel:
```
python2 sike.py -run_full_atk -min_cpus 20 -max_cpus 20 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_32_20_ -pd 14 -no_hag
python2 sike.py -run_full_atk -min_cpus 20 -max_cpus 20 -m p_36_22 -min_mem 10 -max_mem 10 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_36_22_ -pd 16 -no_hag
python2 sike.py -run_full_atk -min_cpus 20 -max_cpus 20 -m p_40_25 -min_mem 11 -max_mem 11 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_40_25_ -pd 18 -no_hag
python2 sike.py -run_full_atk -min_cpus 20 -max_cpus 20 -m p_44_27 -min_mem 13 -max_mem 13 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_44_27_ -pd 20 -no_hag
python2 sike.py -run_full_atk -min_cpus 40 -max_cpus 40 -m p_48_30 -min_mem 13 -max_mem 13 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_48_30_ -pd 22 -no_hag
python2 sike.py -run_full_atk -min_cpus 40 -max_cpus 40 -m p_52_33 -min_mem 15 -max_mem 15 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_52_33_ -pd 24 -no_hag
python2 sike.py -run_full_atk -min_cpus 40 -max_cpus 40 -m p_56_35 -min_mem 17 -max_mem 17 -iterations 1000 -seed 1337 -server solardiesel_reproducing_table_3_adj_p_56_35_ -pd 26 -no_hag
```

Output files:

```
solardiesel_reproducing_table_3_adj_p_32_20_sike_full_atk_True_hag_False_precomp_14
solardiesel_reproducing_table_3_adj_p_36_22_sike_full_atk_True_hag_False_precomp_16
solardiesel_reproducing_table_3_adj_p_40_25_sike_full_atk_True_hag_False_precomp_18
solardiesel_reproducing_table_3_adj_p_44_27_sike_full_atk_True_hag_False_precomp_20
solardiesel_reproducing_table_3_adj_p_48_30_sike_full_atk_True_hag_False_precomp_22
solardiesel_reproducing_table_3_adj_p_52_33_sike_full_atk_True_hag_False_precomp_24
solardiesel_reproducing_table_3_adj_p_56_35_sike_full_atk_True_hag_False_precomp_26
```

To compare to the above instances include corresponding `gen` instances (`nbits_state = ceil(eA/2. + log(3,2))` to get close to the SIDH ones):

```
             nbits_state        log(w)
 p32_20        18               9
 p36_22        20              10
 p40_25        22              11
 p44_27        24              13
```

Done (using xxhash and aes-ni):

```
python2 gen.py -run_full_atk -min_cpus 20 -max_cpus 20 -min_nbits 18 -max_nbits 18 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_3_Adj_et_al_using_gen_ -no_hag
python2 gen.py -run_full_atk -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_3_Adj_et_al_using_gen_ -no_hag
python2 gen.py -run_full_atk -min_cpus 20 -max_cpus 20 -min_nbits 22 -max_nbits 22 -min_mem 11 -max_mem 11 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_3_Adj_et_al_using_gen_ -no_hag
python2 gen.py -run_full_atk -min_cpus 20 -max_cpus 20 -min_nbits 24 -max_nbits 24 -min_mem 13 -max_mem 13 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_3_Adj_et_al_using_gen_ -no_hag
```

Output files:

```
atomkohle_reproducing_table_3_Adj_et_al_using_gen_gen_full_atk_True_hag_False
atomkohle_reproducing_table_3_Adj_et_al_using_gen_aes_ni_xof_gen_full_atk_True_hag_False
```

## New experiments

We run the attack on a variety of parameters, to enable comparing the behavior of the various quantities as a function of the number of CPUs and size of the memory.
Also we show the impact of precomputation and other improvements.

### Generic random function

Note: 1 fv will *not* show the linear speedup in cycles/wall_time due to the dist_cols tree being built.

Done on Solardiesel (using xxhash and aes-ni xof):

```
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 16 -max_nbits 16 -min_mem 8 -max_mem 8 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 18 -max_nbits 18 -min_mem 9 -max_mem 9 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 22 -max_nbits 22 -min_mem 11 -max_mem 11 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 24 -max_nbits 24 -min_mem 12 -max_mem 12 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 26 -max_nbits 26 -min_mem 13 -max_mem 13 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 28 -max_nbits 28 -min_mem 14 -max_mem 14 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
```

Done on Solardiesel (using xxhash and aes-ni xof):

```
python2 gen.py -min_cpus 2 -max_cpus 18 -min_nbits 16 -max_nbits 16 -min_mem 8 -max_mem 8 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 2 -max_cpus 18 -min_nbits 18 -max_nbits 18 -min_mem 9 -max_mem 9 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 2 -max_cpus 18 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 2 -max_cpus 18 -min_nbits 22 -max_nbits 22 -min_mem 11 -max_mem 11 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 2 -max_cpus 18 -min_nbits 24 -max_nbits 24 -min_mem 12 -max_mem 12 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 2 -max_cpus 18 -min_nbits 26 -max_nbits 26 -min_mem 13 -max_mem 13 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 2 -max_cpus 18 -min_nbits 28 -max_nbits 28 -min_mem 14 -max_mem 14 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
python2 gen.py -min_cpus 2 -max_cpus 18 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 1337 -step 4 -server some_gen_experiments_
```

Output files:

```
some_gen_experiments_gen_full_atk_False_hag_False
some_gen_experiments_gen_full_atk_False_hag_True_crumbs_default
some_gen_experiments_aes_ni_xof_gen_full_atk_False_hag_False
some_gen_experiments_aes_ni_xof_gen_full_atk_False_hag_True
```

Done on Azure (using xxhash):

```
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 16 -max_nbits 16 -min_mem 8 -max_mem 8 -iterations 1000 -seed 1337 -step 4 -server some_gen_experiments_ -run_full_atk
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 18 -max_nbits 18 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1337 -step 4 -server some_gen_experiments_ -run_full_atk
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 1000 -seed 1337 -step 4 -server some_gen_experiments_ -run_full_atk
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 22 -max_nbits 22 -min_mem 11 -max_mem 11 -iterations 1000 -seed 1337 -step 4 -server some_gen_experiments_ -run_full_atk
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 24 -max_nbits 24 -min_mem 12 -max_mem 12 -iterations 1000 -seed 1337 -step 4 -server some_gen_experiments_ -run_full_atk
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 26 -max_nbits 26 -min_mem 13 -max_mem 13 -iterations 1000 -seed 1337 -step 4 -server some_gen_experiments_ -run_full_atk
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 28 -max_nbits 28 -min_mem 14 -max_mem 14 -iterations 1000 -seed 1337 -step 4 -server some_gen_experiments_ -run_full_atk
python2 gen.py -min_cpus 4 -max_cpus 20 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 1000 -seed 1337 -step 4 -server some_gen_experiments_ -run_full_atk
```

Output files:

```
azure_some_gen_experiments_gen_full_atk_True_hag_False
azure_some_gen_experiments_gen_full_atk_True_hag_True
```

#### Fewer, larger experiments

Aes-ni xof on Solardiesel:

```
python2 gen.py -min_cpus 20 -max_cpus 40 -min_nbits 40 -max_nbits 52 -iterations 64 -seed 1337 -step 4 -server some_large_gen_experiments_`
```

Output files:

```
some_large_gen_experiments_aes_ni_xof_gen_full_atk_False_hag_False
some_large_gen_experiments_aes_ni_xof_gen_full_atk_False_hag_True
```

#### Crumbs experiments

Done (using xxhash, with aes-ni using seed 1234321):

```
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -no_hag
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 1
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 2
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 3
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 4
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 5
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 6
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 7
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 8
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 9
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 10
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 11
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 12
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 13
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 14
python2 gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 15
```

Done (using xxhash, with aes-ni using seed 1234321):

```
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -no_hag
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 5
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 10
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 15
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 20
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 25
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 30
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 35
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 40
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 45
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 50
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 55
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 60
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 65
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 70
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 75
python2 gen.py -min_cpus 28 -max_cpus 28 -min_nbits 30 -max_nbits 30 -min_mem 15 -max_mem 15 -iterations 64 -seed 0xdeadbeef -server atomkohle_hag_gen_ -crumbs 80
```

Output files:

```
atomkohle_hag_gen_n_20_gen_full_atk_False_hag_False
atomkohle_hag_gen_n_20_gen_full_atk_False_hag_True
atomkohle_hag_gen_n_30gen_full_atk_False_hag_False
atomkohle_hag_gen_n_30gen_full_atk_False_hag_True
atomkohle_hag_gen_aes_ni_xof_gen_full_atk_False_hag_False
atomkohle_hag_gen_aes_ni_xof_gen_full_atk_False_hag_True
atomkohle_hag_gen_aes_ni_xof_gen_full_atk_False
```

### SIKE random function

#### Experiments to show wall-time vs cpus

Done using stakhanov, windowed (10), nobiggie. To change strategy, edit `/src/settings.h`:

```
python2 sike.py -run_full_atk -min_cpus 28 -max_cpus 28 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 26 -max_cpus 26 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 24 -max_scpus 24 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 22 -max_cpus 22 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 20 -max_cpus 20 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 18 -max_cpus 18 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 16 -max_cpus 16 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 14 -max_cpus 14 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 12 -max_cpus 12 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 10 -max_cpus 10 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 8 -max_cpus 8 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 6 -max_cpus 6 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 4 -max_cpus 4 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
python2 sike.py -run_full_atk -min_cpus 2 -max_cpus 2 -m p_32_20 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1234321 -server atomkohle_showing_linear_speedup_sike -pd 0 -no_hag
```

Output files:

```
atomkohle_showing_linear_speedup_sike_nobiggie_sike_full_atk_True_hag_False_precomp_0
atomkohle_showing_linear_speedup_sikesike_full_atk_True_hag_False_precomp_0
atomkohle_showing_linear_speedup_sike_windowed_10_sike_full_atk_True_hag_False_precomp_0
```

#### SIKE Precomputation

Only benchmarking the Update function for SIKE for all different precomputation depths in steps of 2:

```
python2 sike_bench_update.py -m p_32_20 -server linuxbox2 -min_delta 0 -max_delta 14 -iterations 1000000
python2 sike_bench_update.py -m p_36_22 -server linuxbox2 -min_delta 0 -max_delta 16 -iterations 1000000
python2 sike_bench_update.py -m p_40_25 -server linuxbox2 -min_delta 0 -max_delta 18 -iterations 1000000
python2 sike_bench_update.py -m p_44_27 -server linuxbox2 -min_delta 0 -max_delta 20 -iterations 1000000
python2 sike_bench_update.py -m p_48_30 -server linuxbox2 -min_delta 0 -max_delta 22 -iterations 1000000
python2 sike_bench_update.py -m p_52_33 -server linuxbox2 -min_delta 0 -max_delta 24 -iterations 1000000
python2 sike_bench_update.py -m p_56_35 -server linuxbox2 -min_delta 0 -max_delta 24 -iterations 1000000
python2 sike_bench_update.py -m p_60_38 -server linuxbox2 -min_delta 0 -max_delta 24 -iterations 1000000
```

Output files:

```
linuxbox2_sike_bench_Update_p_32_20
linuxbox2_sike_bench_Update_p_36_22
linuxbox2_sike_bench_Update_p_40_25
linuxbox2_sike_bench_Update_p_44_27
linuxbox2_sike_bench_Update_p_48_30
linuxbox2_sike_bench_Update_p_52_33
linuxbox2_sike_bench_Update_p_56_35
linuxbox2_sike_bench_Update_p_60_38
```

Same, but higher max delta on atomkohle:

```
python2 sike_bench_update.py -m p_32_20 -server atomkohle_precomputation_sike_ -min_delta 0 -max_delta 14 -iterations 1000000
python2 sike_bench_update.py -m p_36_22 -server atomkohle_precomputation_sike_ -min_delta 0 -max_delta 16 -iterations 1000000
python2 sike_bench_update.py -m p_40_25 -server atomkohle_precomputation_sike_ -min_delta 0 -max_delta 18 -iterations 1000000
python2 sike_bench_update.py -m p_44_27 -server atomkohle_precomputation_sike_ -min_delta 0 -max_delta 20 -iterations 1000000
python2 sike_bench_update.py -m p_48_30 -server atomkohle_precomputation_sike_ -min_delta 0 -max_delta 22 -iterations 1000000
python2 sike_bench_update.py -m p_52_33 -server atomkohle_precomputation_sike_ -min_delta 0 -max_delta 24 -iterations 1000000
python2 sike_bench_update.py -m p_56_35 -server atomkohle_precomputation_sike_ -min_delta 0 -max_delta 26 -iterations 1000000
python2 sike_bench_update.py -m p_60_38 -server atomkohle_precomputation_sike_ -min_delta 0 -max_delta 28 -iterations 1000000
```

Output files:

```
atomkohle_precomputation_sike_sike_bench_Update_p_32_20
atomkohle_precomputation_sike_sike_bench_Update_p_36_22
atomkohle_precomputation_sike_sike_bench_Update_p_40_25
atomkohle_precomputation_sike_sike_bench_Update_p_44_27
atomkohle_precomputation_sike_sike_bench_Update_p_48_30
atomkohle_precomputation_sike_sike_bench_Update_p_52_33
atomkohle_precomputation_sike_sike_bench_Update_p_56_35
atomkohle_precomputation_sike_sike_bench_Update_p_60_38
```

#### Larger prime experiments

Output files:

```
p434_atomkohlesike_full_atk_False_hag_False_precomp_16
p434_atomkohle_w_18_sike_full_atk_False_hag_False_precomp_16
p434_atomkohle_w_20_sike_full_atk_False_hag_False_precomp_16
```