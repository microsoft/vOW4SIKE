from math import sqrt

def extract_stats(experiment, params):
    # algorithm properties (for plotting expected quantities)
    w = float(2 ** params["memory_log_size"])
    n = float(2 ** params["nbits_state"])
    m = float(params["n_threads"])
    theta = params["alpha"] * sqrt(w / n)
    # non_distinct_collisions = n / 2
    single_f_runtime = sqrt(n*w) # O()
    
    # experimental results in [vOW98]
    exp_i = 2.5 * sqrt(n**3 / w)
    exp_collisions = 1.3 * w
    exp_distinct_collisions = 1.1 * w
    exp_function_versions = 0.45 * n / w
    exp_distinguished_points = 4.5 * n

    experiment["expected"]["num_steps"] = exp_i
    experiment["expected"]["cycles"] = exp_i / m
    experiment["expected"]["wall_time"] = exp_i / m
    experiment["expected"]["num_steps_collect"] = exp_i * .8
    experiment["expected"]["num_steps_locate"] = exp_i * .2
    experiment["expected"]["avg_random_functions"] = exp_function_versions
    experiment["expected"]["collisions"] = exp_collisions * experiment["expected"]["avg_random_functions"]
    experiment["expected"]["dist_cols"] = exp_distinct_collisions * experiment["expected"]["avg_random_functions"]
    experiment["expected"]["dist_points"] = exp_distinguished_points
    experiment["expected"]["ratio_coll_per_fun"] = 1.
    experiment["expected"]["ratio_dist_cols_per_fun"] = 1.
    experiment["expected"]["ratio_avg_random_functions"] = 1.
    experiment["expected"]["ratio_num_steps"] = 1.
    experiment["expected"]["coll_per_fun"] = experiment["expected"]["collisions"] / experiment["expected"]["avg_random_functions"]
    experiment["expected"]["dist_cols_per_fun"] = experiment["expected"]["dist_cols"] / experiment["expected"]["avg_random_functions"]
    if not params["run_full_atk"]:
        # only one function
        experiment["expected"]["num_steps"] /= experiment["expected"]["avg_random_functions"]
        experiment["expected"]["num_steps_collect"] /= experiment["expected"]["avg_random_functions"]
        experiment["expected"]["num_steps_locate"] /= experiment["expected"]["avg_random_functions"]
        experiment["expected"]["collisions"] /= experiment["expected"]["avg_random_functions"]
        experiment["expected"]["dist_cols"] /= experiment["expected"]["avg_random_functions"]
        experiment["expected"]["dist_points"] /= experiment["expected"]["avg_random_functions"]
        experiment["expected"]["cycles"] /= experiment["expected"]["avg_random_functions"]
        experiment["expected"]["wall_time"] /= experiment["expected"]["avg_random_functions"]
        experiment["expected"]["avg_random_functions"] = 1

    if not params["run_full_atk"]:
        # last function may finish quickly if running full atk
        experiment["coll_per_fun"] = experiment["collisions"]
        experiment["dist_cols_per_fun"] = experiment["dist_cols"]
        experiment["ratio_coll_per_fun"] = experiment["coll_per_fun"] / experiment["expected"]["coll_per_fun"]
        experiment["ratio_dist_cols_per_fun"] = experiment["dist_cols_per_fun"] / experiment["expected"]["dist_cols_per_fun"]

    # ratios wrt expected value
    experiment["ratio_avg_random_functions"] = experiment["avg_random_functions"] / experiment["expected"]["avg_random_functions"]
    experiment["ratio_num_steps"] = experiment["num_steps"] / experiment["expected"]["num_steps"]
    experiment["ratio_wall_time"] = experiment["wall_time"] / experiment["expected"]["wall_time"]
    experiment["ratio_cycles"] = experiment["cycles"] / experiment["expected"]["cycles"]