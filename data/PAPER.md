# Instructions to generate data presented in [6] from raw output

The `/data/raw_data` directory contains the output of the experiments run during write-up of the paper.
Some settings and code changes happened since the original experiments, and the experiment seeds do not result in the exact same raw files. Furthermore, the experiments being parallel further affects exact reproducibility. Yet, running the commands in [EXPERIMENTS.md](EXPERIMENTS.md) does result in the same observed behaviour.

## Dependencies

- Sagemath 8
  - The experiments were run across 2018-2019, and hence the code relies on
    on Sagemath running over Python 2.7. Any version of Sage 8 should do, we recommend 8.9.

- Pandas
  - To be installed inside of Sagemath, using 

    ```bash
    $ sage -sh
    (sage-sh) $ pip install pandas
    (sage-sh) $ exit
    ```

## Generating the tables and figures

To generate the tables and figures in the paper from the data in `raw_data`, run the following code.

```python
$ sage generate_paper_data.py
```

This will result in two new directories, `tables` and `figures`.

### Tables

- Table 1: data can be found in `tables/adj_table_3` (expected values for SIDH should be generated from the Table's caption).
- Table 2: data can be found in `tables/precomputation_table`.
- Table 3: data can be generated following the same steps described here from the [`/cpp/data`](../cpp/data/) directory and looking into `/cpp/tables/adj_table_3`.
- Table 4: data can be found in `tables/vow_table_1`.
- Table 5 and 6: values are extrapolated from benchmarks on the [SIDH](https://github.com/Microsoft/PQCrypto-SIDH) library.
- Table 7 (and its interpolation): can be generated running `sage p434.sage`.
- Table 8: data can be found in `tables/adj_table_3_gen`.
- Table 9: data can be found in `tables/adj_table_3_gen`.

### Figures

Figures are generated as `.pgf` files, together with an automatically generated surrounding `.tex` file (include paths are off if the files are taken stand-alone).

- Figure 4: picture can be found in `figures/crumb_plots/fig_crumbs-n-30-num_steps_locate.tex`.
- Figure 5: picture can be found in `figures/sike_vs_cpus/fig_sike-p_32_20-wall_time_inverse-stakhanov.tex`.
- Figure 6: picture can be found in `figures/gen_vs_cpus/fig_gen-n-52-w-13-wall_time-stakhanov.tex`.
- Figure 7: picture can be found in `figures/gen_vs_cpus/fig_gen-n-52-w-13-num_steps-stakhanov.tex`.
- Figure 8: picture can be found in `figures/sike_vs_cpus/fig_sike-p_32_20-wall_time_inverse-windowed.tex`.
- Figure 9: picture can be found in `figures/sike_vs_cpus/fig_sike-p_32_20-wall_time_inverse-nobiggie.tex`.