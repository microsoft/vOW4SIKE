# Experiment list

The following experiments are run on a generic XOF based on AES-NI.

The cited output files can be found in `/cpp/data/raw_data/`.

## Reproducing the results by Adj et al. [5]

### Table 2

```
python gen.py -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_2_Adj_et_al_ -no_hag
python gen.py -min_cpus 20 -max_cpus 20 -min_nbits 24 -max_nbits 24 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_2_Adj_et_al_ -no_hag
python gen.py -min_cpus 20 -max_cpus 20 -min_nbits 28 -max_nbits 28 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_2_Adj_et_al_ -no_hag
python gen.py -min_cpus 20 -max_cpus 20 -min_nbits 32 -max_nbits 32 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_2_Adj_et_al_ -no_hag
python gen.py -min_cpus 20 -max_cpus 20 -min_nbits 36 -max_nbits 36 -min_mem 2 -max_mem 18 -step 1 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_2_Adj_et_al_ -no_hag
```

### Table 3

```
python gen.py -run_full_atk -min_cpus 20 -max_cpus 20 -min_nbits 18 -max_nbits 18 -min_mem 9 -max_mem 9 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_3_Adj_et_al_using_gen_ -no_hag
python gen.py -run_full_atk -min_cpus 20 -max_cpus 20 -min_nbits 20 -max_nbits 20 -min_mem 10 -max_mem 10 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_3_Adj_et_al_using_gen_ -no_hag
python gen.py -run_full_atk -min_cpus 20 -max_cpus 20 -min_nbits 22 -max_nbits 22 -min_mem 11 -max_mem 11 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_3_Adj_et_al_using_gen_ -no_hag
python gen.py -run_full_atk -min_cpus 20 -max_cpus 20 -min_nbits 24 -max_nbits 24 -min_mem 13 -max_mem 13 -iterations 1000 -seed 1337 -server atomkohle_reproducing_table_3_Adj_et_al_using_gen_ -no_hag
```
