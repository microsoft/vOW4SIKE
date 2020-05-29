from __future__ import print_function
import json
import os, sys, os.path
import pandas
import matplotlib as mpl
mpl.use('pgf')
pgf_with_pdflatex = {
    "pgf.texsystem": "pdflatex",
    "pgf.preamble": [
        r"\usepackage[utf8x]{inputenc}",
        r"\usepackage[T1]{fontenc}",
        # r"\usepackage{cmbright}",
    ],
    "font.family": "serif",
    "font.serif": [],                   # use latex default serif font
    "font.size": "14.0"
}
mpl.rcParams.update(pgf_with_pdflatex)
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter
axis_formatter = ScalarFormatter()
axis_formatter.set_scientific(True)
axis_formatter.set_powerlimits((-2, 2))
from sage.all import mean, median, RR, show, line, sqrt


def parse(filename):
    """
    Utility code that parses (possibly corrupted) experiment output and packages
    it as a a Sagemath sobj binary file.
    """
    with open(filename) as f:
        lines = f.readlines()

    experiments = {}

    for line in lines:
        try:
            obj = json.loads(line)
        except:
            # since the file was generated improperly (due to process being killed at points)
            # some lines won't be valid json. just ignore them
            continue
        nbits, mem, cpus = tuple(obj['k'])
        result = obj['v']
        if nbits not in experiments:
            experiments[nbits] = {}
        if mem not in experiments[nbits]:
            experiments[nbits][mem] = {}
        experiments[nbits][mem][cpus] = result
    return experiments


def generating_adj_table_2(inputdir="./raw_data", outputdir="../../../paper/tables", aes_ni=True):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/adj_table_2" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    if aes_ni:
        filename = "atomkohle_aes_xof_reproducing_table_2_Adj_et_al_gen_full_atk_False_hag_False"
        machine = "atomkohle"
        label = "table:adj-table-2-aes-ni"
        xof = "AES-based"
    else:
        filename = "solardiesel_reproducing_table_2_Adj_et_al_gen_full_atk_False_hag_False"
        machine = "solardiesel"
        label = "table:adj-table-2-xxhash"
        xof = "xxHash-based"
    filename = "%s/%s" % (inputdir, filename)


    data = parse(filename=filename)
    state_lengths = data.keys()
    mem_lengths = data[state_lengths[0]].keys()
    cpus = data[state_lengths[0]][mem_lengths[0]].keys()[0]
    iterations = data[state_lengths[0]][mem_lengths[0]][cpus]["total_number"]

    caption = "Reproduction of Table 2 from~\\cite{canadians}, using the %s XOF on \\%s. Experiments are averaged over %d function versions using %d cores." % (
        xof,
        machine,
        iterations,
        cpus
    )

    table = "\\begin{table}\n"
    table += "\t\\caption{%s}\n\t\\centering\n" % caption
    table += "\t\\label{%s}\n" % label
    table += "\t\\renewcommand{\\tabcolsep}{0.05in}\n"
    table += "\t\\begin{tabular}{l" + ("r" * len(mem_lengths)) + "}\n"
    table += "\t\t\\toprule\n"
    table += "\t\t$w$ " + "".join([ "& $2^{%s}$ " % str(membits) for membits in sorted(mem_lengths) ]) + "\\\\ \\midrule\n"

    for M in sorted(data.keys()):
        table += "\t\t\\multicolumn{%d}{c}{$M = 2^{%d}$} \\\\\n" % (len(mem_lengths) + 1, M)
        table += "\t\t$c_1$ " + "".join(["& $%.2f$ " % (data[M][w][cpus]["coll_per_fun"] / 2**w) for w in sorted(data[M].keys())]) + "\\\\\n"
        table += "\t\t$c_2$ " + "".join(["& $%.2f$ " % (data[M][w][cpus]["dist_cols_per_fun"] / 2**w) for w in sorted(data[M].keys())]) + "\\\\"
        if M == sorted(data.keys())[-1]:
            table += "\\bottomrule\n"
        else:
            table += "\\midrule\n"

    table += "\t\\end{tabular}\n\\end{table}\n"

    with open("%s/%s.tex" % (outputdir, label.replace(':', '_')), 'w') as f:
        f.write(table)


def generating_adj_table_3(structure, inputdir="./raw_data", outputdir="../../../paper/tables"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/adj_table_3" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    # for using gen we use a different function since the loops are a bit different
    if structure == "sidh":
        filenames = [
            "%s/solardiesel_reproducing_table_3_adj_p_32_20_sidh_full_atk_True_hag_False_precomp_True" % inputdir,
            "%s/solardiesel_reproducing_table_3_adj_p_36_22_sidh_full_atk_True_hag_False_precomp_True" % inputdir,
            "%s/solardiesel_reproducing_table_3_adj_p_40_25_sidh_full_atk_True_hag_False_precomp_True" % inputdir,
            "%s/solardiesel_reproducing_table_3_adj_p_44_27_sidh_full_atk_True_hag_False_precomp_True" % inputdir
        ]
        machine = "solardiesel"
        label = "table:adj-table-3-sidh"
    elif structure == "sike":
        filenames = [
            "%s/solardiesel_reproducing_table_3_adj_p_32_20_sike_full_atk_True_hag_False_precomp_14" % inputdir,
            "%s/solardiesel_reproducing_table_3_adj_p_36_22_sike_full_atk_True_hag_False_precomp_16" % inputdir,
            "%s/solardiesel_reproducing_table_3_adj_p_40_25_sike_full_atk_True_hag_False_precomp_18" % inputdir,
            "%s/solardiesel_reproducing_table_3_adj_p_44_27_sike_full_atk_True_hag_False_precomp_20" % inputdir,
            "%s/solardiesel_reproducing_table_3_adj_p_48_30_sike_full_atk_True_hag_False_precomp_22" % inputdir,
            "%s/solardiesel_reproducing_table_3_adj_p_52_33_sike_full_atk_True_hag_False_precomp_24" % inputdir,
            "%s/solardiesel_reproducing_table_3_adj_p_56_35_sike_full_atk_True_hag_False_precomp_26" % inputdir
        ]
        machine = "solardiesel"
        label = "table:adj-table-3-sike"

    table = "\\begin{table}\n"

    # how many cpus?
    data = parse(filename=filenames[0])
    cpus = data[data.keys()[0]][data[data.keys()[0]].keys()[0]].keys()[0]

    caption = "Reproduction of Table 3 from~\\cite{canadians}, using our SIDH code and isogeny precomputation on \\%s. Experiments are run using %d cores." % (
        machine,
        cpus
    )

    table += "\t\\caption{%s}\n" % caption
    table += "\t\\renewcommand{\\tabcolsep}{0.05in}\n"
    table += "\t\\renewcommand{\\arraystretch}{1.3}\n"
    table += "\t\\centering\n"
    table += "\t\\begin{tabular}{ccccc|cc|ccc|ccc}\n"
    table += "\t\t\\toprule\n"
    table += "\t\t & & & & & \\multicolumn{2}{c}{expected} & \\multicolumn{3}{c}{average} & \\multicolumn{3}{c}{median}\\\\\n"
    table += "\t\t\\(\\etwo\\) & \\(\\ethr\\) & \\(d\\) & \\(\\log w\\) & \\#runs & \n"
    table += "\t\t\\(\\#f_n\\) & \\(\\log\\sqrt{|S|^3/w}\\) &\n"
    table += "\t\t\\(\\#f_n\\) & \\(\\log\\sqrt{|S|^3/w}\\) & total cycles &\n"
    table += "\t\t\\(\\#f_n\\) & \\(\\log\\sqrt{|S|^3/w}\\) & total cycles\\\\ \\midrule\n"

    cofactors = {
        'p_32_20': 23,
        'p_34_21': 109,
        'p_36_22': 31,
        'p_38_23': 271,
        'p_40_25': 71,
        'p_42_26': 37,
        'p_44_27': 37,
        'p_48_30': -1,
        'p_52_33': -1,
        'p_56_35': -1
    }

    # loop goes here
    for filename in filenames:
        data = parse(filename=filename)
        modulus = data.keys()[0]
        mem_bits = data[modulus].keys()[0]
        cpus = data[modulus][mem_bits].keys()[0]
        
        _, e_2, e_3 = modulus.split('_')
        d = cofactors[modulus]

        table += "\t\t$%s$ & $%s$ & $%d$ & $%d$ & $%d$ & " % (
            e_2, e_3, d, mem_bits, data[modulus][mem_bits][cpus]['total_number']
        )

        random_functions = [ el['avg_random_functions'] for el in data[modulus][mem_bits][cpus]['full_data'] ]
        num_steps = [ el['num_steps'] for el in data[modulus][mem_bits][cpus]['full_data'] ]
        cycles = [ el['cycles'] for el in data[modulus][mem_bits][cpus]['full_data'] ]

        # expected
        table += "$%.2f$ & $%.2f$ & " % (
            data[modulus][mem_bits][cpus]['expected']['avg_random_functions'],
            RR(data[modulus][mem_bits][cpus]['expected']['num_steps']).log(2),
        )

        # averages
        table += "$%.2f$ & $%.2f$ & $%.2f$ & " % (
            mean(random_functions),
            mean(num_steps).log(2),
            mean(cycles).log(2)
        )

        # medians
        table += "$%.2f$ & $%.2f$ & $%.2f$ \\\\\n" % (
            median(random_functions),
            median(num_steps).log(2),
            median(cycles).log(2)
        )

    table += "\t\t\\bottomrule\n"
    table += "\t\\end{tabular}\n"
    table += "\\end{table}\n"

    with open("%s/%s.tex" % (outputdir, label.replace(':', '_')), 'w') as f:
        f.write(table)


def generating_adj_table_3_gen(aes_ni=True, inputdir="./raw_data", outputdir="../../../paper/tables"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/adj_table_3_gen" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    if aes_ni:
        filename = "atomkohle_reproducing_table_3_Adj_et_al_using_gen_aes_ni_xof_gen_full_atk_True_hag_False"
        machine = "atomkohle"
        xof = "AES-based"
        label = "table:adj-table-3-aes-ni"
    else:
        filename = "atomkohle_reproducing_table_3_Adj_et_al_using_gen_gen_full_atk_True_hag_False"
        machine = "atomkohle"
        xof = "xxHash-based"
        label = "table:adj-table-3-xxhash"
    filename = "%s/%s" % (inputdir, filename)

    table = "\\begin{table}\n"

    # how many cpus?
    data = parse(filename=filename)
    state_lengths = sorted(data.keys())
    cpus = data[state_lengths[0]][data[state_lengths[0]].keys()[0]].keys()[0]

    caption = "Reproduction of Table 3 from~\\cite{canadians}, using our a %s generic random function on \\%s. Experiments are run using %d cores." % (
        xof,
        machine,
        cpus
    )

    table += "\t\\caption{%s}\n" % caption
    table += "\t\\label{%s}\n" % label
    table += "\t\\renewcommand{\\tabcolsep}{0.05in}\n"
    table += "\t\\renewcommand{\\arraystretch}{1.3}\n"
    table += "\t\\centering\n"
    table += "\t\\begin{tabular}{ccc|cc|ccc|ccc}\n"
    table += "\t\t\\toprule\n"
    table += "\t\t & & & \\multicolumn{2}{c}{expected} & \\multicolumn{3}{c}{average} & \\multicolumn{3}{c}{median}\\\\\n"
    table += "\t\t\\(\\log{|S|}\\) & \\(\\log w\\) & \\#runs & \n"
    table += "\t\t\\(\\#f_n\\) & \\(\\log\\sqrt{|S|^3/w}\\) &\n"
    table += "\t\t\\(\\#f_n\\) & \\(\\log\\sqrt{|S|^3/w}\\) & cycles &\n"
    table += "\t\t\\(\\#f_n\\) & \\(\\log\\sqrt{|S|^3/w}\\) & cycles\\\\ \\midrule\n"

    # loop goes here
    for nbits_state in state_lengths:
        mem_bits = data[nbits_state].keys()[0]
        cpus = data[nbits_state][mem_bits].keys()[0]
        
        table += "\t\t$%d$ & $%d$ & $%d$ & " % (
            nbits_state, mem_bits, data[nbits_state][mem_bits][cpus]['total_number']
        )

        random_functions = [ el['avg_random_functions'] for el in data[nbits_state][mem_bits][cpus]['full_data'] ]
        num_steps = [ el['num_steps'] for el in data[nbits_state][mem_bits][cpus]['full_data'] ]
        cycles = [ el['cycles'] for el in data[nbits_state][mem_bits][cpus]['full_data'] ]

        # expected
        table += "$%.2f$ & $%.2f$ & " % (
            data[nbits_state][mem_bits][cpus]['expected']['avg_random_functions'],
            RR(data[nbits_state][mem_bits][cpus]['expected']['num_steps']).log(2),
        )

        # averages
        table += "$%.2f$ & $%.2f$ & $%.2f$ & " % (
            mean(random_functions),
            mean(num_steps).log(2),
            mean(cycles).log(2)
        )

        # medians
        table += "$%.2f$ & $%.2f$ & $%.2f$ \\\\\n" % (
            median(random_functions),
            median(num_steps).log(2),
            median(cycles).log(2)
        )

    table += "\t\t\\bottomrule\n"
    table += "\t\\end{tabular}\n"
    table += "\\end{table}\n"

    with open("%s/%s.tex" % (outputdir, label.replace(':', '_')), 'w') as f:
        f.write(table)


def generating_crumb_plots(aes_ni=True, inputdir="./raw_data", outputdir="../../../paper/figures"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/crumb_plots" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    def stat_vs_crumbs(
        experiments,
        figure_filename,
        state_bits=0,
        mem_bits=0,
        statistic="", # selector(sorted([stat for stat in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]].keys() if (stat != "expected" and stat != "full_data")]), buttons=True),
        box_plots=True,
        y_min="function min", # or 0 
        inverse=False,
        expected=True,
    ): 
        if state_bits not in experiments:
            raise ValueError("state_bits %d not available" % state_bits)
        
        if mem_bits not in experiments[state_bits]:
            raise ValueError("mem_bits %d not available" % mem_bits)
        
        inv_exp = -1. if inverse else 1.
        
        results = experiments[state_bits][mem_bits]
        
        if y_min != 0:
            y_min = None
            
        if box_plots:
            import platform
            if platform.system() != 'Linux':
                raise ValueError("box plots only available on Linux")

            if statistic not in results[results.keys()[0]]["full_data"][0]:
                raise ValueError("box plot not available for %s" % statistic)

            transpose_list = lambda l: map(list, zip(*l))
            
            # need to do a max with expected
            # dist_cols is way too low (== 0, not being collected) in full attack and too high in 1fv!!
            
            fig, ax = plt.subplots()

            # format y axis
            ax.yaxis.set_major_formatter(axis_formatter) 
            
            # plot average
            ax.plot(range(1, len(results.keys())+1), [results[cpus][statistic] ** inv_exp for cpus in sorted(results.keys())], 'y', linewidth=2)
            ax.legend(['average'+ (" inverse" if inverse else "")])
            
            # plot expected
            if statistic in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]]["expected"] and expected:
                ax.plot(range(1, len(results.keys())+1), [results[cpus]["expected"][statistic] ** inv_exp for cpus in sorted(results.keys())], 'g', linewidth=2)
                ax.legend(['average'+ (" inverse" if inverse else ""), 'expected' + (" inverse" if inverse else "") + " for 0 crumbs"])
            
            # plot boxplot
            data = [ [full_data[statistic] ** inv_exp for full_data in results[cpus]["full_data"] ] for cpus in sorted(results.keys())]
            df = pandas.DataFrame(transpose_list(data), columns=sorted(results.keys())).astype(float)
            df.plot.box(grid=True, ylim=(y_min if y_min is not None else df.min().min() * .95, df.max().max() * 1.05), ax=ax)
            ax.set_xlabel("max crumbs")
            ax.set_ylabel(("${\\textnormal{" if inverse else "") + statistic.replace("_", " ") + ("}}^{-1}$" if inverse else ""))
            plt.savefig(figure_filename)
            plt.close()
        else:
            f = [(cpus, results[cpus][statistic] ** inv_exp) for cpus in sorted(results.keys())]
            g = line(f, legend_label=str("measured "+ ("inverse " if inverse else "")+statistic))
            try:
                if statistic in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]]["expected"] and expected:
                    e_f = [(cpus, results[cpus]["expected"][statistic] ** inv_exp) for cpus in sorted(results.keys())]
                    g += line(e_f, rgbcolor=(1, 0, 0), legend_label=str("expected "+ ("inverse " if inverse else "")+statistic))
            except:
                pass
            show(g, axes_labels=['$max crumbs$', statistic], ymin=y_min)
    
    if aes_ni:
        filename = "atomkohle_hag_gen_aes_ni_xof_gen_full_atk_False"
        machine = "atomkohle"
        xof = "AES-based"
    else:
        raise ValueError("have to merge the files first")
        machine = "#"
        xof = "xxHash-based"
    filename = "%s/%s" % (inputdir, filename)

    lines = []
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    experiments = {}
    for line in lines:
        data = json.loads(line)
        nbits_state, mem_bits, cpus = data['k']
        crumbs = data['v']['max_crumbs']
        
        if nbits_state not in experiments:
            experiments[nbits_state] = {}
        if mem_bits not in experiments[nbits_state]:
            experiments[nbits_state][mem_bits] = {}
        
        experiments[nbits_state][mem_bits][crumbs] = data['v']
        experiments[nbits_state][mem_bits][crumbs]["cpus"] = cpus

    # we now use the "print vs cpus", but use the crumbs as cpus
    for nbits_state in experiments:
        mem_bits = experiments[nbits_state].keys()[0]
        cpus = experiments[nbits_state][mem_bits][0]['cpus']
        iterations = experiments[nbits_state][mem_bits][0]['total_number']
        for statistic in [u'mem_collisions', u'dist_points', u'num_steps_locate', u'collisions', u'num_steps', u'dist_cols', u'num_steps_collect']:
            label = "fig:crumbs-n-%d-%s" % (nbits_state, statistic)
            caption = "Box plot for %s as a function of maximum amount of crumbs allowed, averaged over %s function versions, using %s cores. %s random function with $\\log{|S|} = %d$, $\\log{w} = %d$, run on \\%s." % (
                statistic.replace("_", " "), iterations, cpus, xof, nbits_state, mem_bits, machine
            )
            figure_filename = "%s/%s.pgf" % (outputdir, label.replace(':', '_'))
            stat_vs_crumbs(
                experiments,
                figure_filename,
                state_bits=nbits_state,
                mem_bits=mem_bits,
                statistic=statistic,
                box_plots=True,
                y_min="function min", # or 0 
                inverse=bool(statistic == "wall_time"),
                expected=bool(statistic in experiments[nbits_state][mem_bits][0]['expected']) and statistic not in ["cycles", "wall_time"],
            )

            figure = "\\begin{figure}[H]\n"
            figure += "\t\\centering\n"
            figure += "\t\\caption{%s}\n" % caption
            figure += "\t\\label{%s}\n" % label
            figure += "\t\\resizebox{.9\\linewidth}{!}{\n"
            figure += "\t\t\\input{%s}\n" % figure_filename[figure_filename.index("figures"):]
            figure += "\t}\n"
            figure += "\\end{figure}"
            
            with open("%s/%s.tex" % (outputdir, label.replace(':', '_')), 'w') as f:
                f.write(figure)


def generating_sike_vs_cpus(sync_strategy, inputdir="./raw_data", outputdir="../../../paper/figures"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/sike_vs_cpus" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    def stat_vs_cpus(
        experiments,
        figure_filename,
        modulus,
        mem_bits,
        statistic,
        box_plots=True,
        y_min="function min", # 0
        inverse=False,
        expected=True,
    ): 
        if mem_bits not in experiments[modulus]:
            raise ValueError("mem_bits %d not available" % mem_bits)
        
        inv_exp = -1. if inverse else 1.
        
        results = experiments[modulus][mem_bits]
        
        if y_min != 0:
            y_min = None
            
        if box_plots:
            import platform
            if platform.system() != 'Linux':
                raise ValueError("box plots only available on Linux")

            if statistic not in results[results.keys()[0]]["full_data"][0]:
                raise ValueError("box plot not available for %s" % statistic)
            
            transpose_list = lambda l: map(list, zip(*l))
            
            # need to do a max with expected
            # dist_cols is way too low (== 0, not being collected) in full attack and too high in 1fv!!
            
            fig, ax = plt.subplots()

            # format y axis
            ax.yaxis.set_major_formatter(axis_formatter) 

            # plot average
            ax.plot(range(1, len(results.keys())+1), [results[cpus][statistic] ** inv_exp for cpus in sorted(results.keys())], 'y', linewidth=2)
            ax.legend(['average'+ (" inverse" if inverse else "")])
            
            # plot expected
            if statistic in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]]["expected"] and expected:
                ax.plot(range(1, len(results.keys())+1), [results[cpus]["expected"][statistic] ** inv_exp for cpus in sorted(results.keys())], 'g', linewidth=2)
                ax.legend(['average'+ (" inverse" if inverse else ""), 'expected'+ (" inverse" if inverse else "")])
            
            # plot boxplot
            data = [ [full_data[statistic] ** inv_exp for full_data in results[cpus]["full_data"] ] for cpus in sorted(results.keys())]
            df = pandas.DataFrame(transpose_list(data), columns=sorted(results.keys())).astype(float)

            df.plot.box(grid=True, ax=ax, showfliers=False)
            # df.plot.box(grid=True, ylim=(y_min if y_min is not None else df.min().min() * .95, df.max().max() * 1.05), ax=ax, showfliers=False)
            # import pdb
            # pdb.set_trace()
            ax.set_xlabel("$m = $ cores")
            ax.set_ylabel(("${\\textrm{" if inverse else "") + statistic.replace("_", " ") + ("}}^{-1}$" if inverse else ""))
            plt.savefig(figure_filename)
            plt.close()
        else:
            f = [(cpus, results[cpus][statistic] ** inv_exp) for cpus in sorted(results.keys())]
            g = line(f, legend_label=str("measured "+ ("inverse " if inverse else "")+statistic))
            try:
                if statistic in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]]["expected"] and expected:
                    e_f = [(cpus, results[cpus]["expected"][statistic] ** inv_exp) for cpus in sorted(results.keys())]
                    g += line(e_f, rgbcolor=(1, 0, 0), legend_label=str("expected " + ("inverse " if inverse else "")+statistic))
            except:
                pass
            show(g, axes_labels=['$m = cores$', statistic], ymin=y_min)
    
    if sync_strategy == "windowed":
        filename = "atomkohle_showing_linear_speedup_sike_windowed_10_sike_full_atk_True_hag_False_precomp_0"
        machine = "atomkohle"
    elif sync_strategy == "stakhanov":
        filename = "atomkohle_showing_linear_speedup_sikesike_full_atk_True_hag_False_precomp_0"
        machine = "atomkohle"
    elif sync_strategy == "nobiggie":
        filename = "atomkohle_showing_linear_speedup_sike_nobiggie_sike_full_atk_True_hag_False_precomp_0"
        machine = "atomkohle"
    else:
        raise ValueError("sync strategy %s not available" % sync_strategy)
    filename = "%s/%s" % (inputdir, filename)

    data = parse(filename=filename)
    modulus = data.keys()[0]
    mem_bits = data[modulus].keys()[0]
    cpus = sorted(data[modulus][mem_bits].keys())
    iterations = data[modulus][mem_bits][cpus[0]]['total_number']

    # add expected wall_time/cycles to the set
    base_cycles = data[modulus][mem_bits][cpus[0]]['cycles'] * float(cpus[0])
    base_wall_time = data[modulus][mem_bits][cpus[0]]['wall_time'] * float(cpus[0])
    for threads in cpus:
        data[modulus][mem_bits][threads]['expected']['cycles'] = base_cycles / float(threads)
        data[modulus][mem_bits][threads]['expected']['wall_time'] = base_wall_time / float(threads)

    for statistic in [u'avg_random_functions', u'mem_collisions', u'dist_points', u'num_steps_locate', u'collisions', u'num_steps', u'wall_time', u'wall_time_inverse', u'num_steps_collect', u'cycles', u'cycles_inverse']:
        label = "fig:sike-%s-%s-%s" % (modulus, statistic, sync_strategy)
        caption = "Box plot for %s as a function of the number of cores used for the attack, averaged over %s function versions on \\%s. SIKE with $e_2 = %s$, $\\log{w} = %d$ with %s sync strategy, using no precomputation. Outliers are hidden to improve plot scale." % (
            statistic.replace('_inverse', '').replace("_", " "), iterations, machine, modulus.split('_')[1], mem_bits, sync_strategy
        )
        if statistic.replace('_inverse', '') in ["wall_time", "cycles"]:
            caption += " The expected value was computed by picking the average value on the lowest number of cores, and scaling it by the appropriate number for the other number of cores."
        figure_filename = "%s/%s.pgf" % (outputdir, label.replace(':', '_'))
        stat_vs_cpus(
            data,
            figure_filename,
            modulus,
            mem_bits,
            statistic.replace('_inverse', ''),
            box_plots=True,
            y_min="function min", # or 0 
            inverse=bool("_inverse" in statistic),
            expected=bool(statistic.replace('_inverse', '') in data[modulus][mem_bits][cpus[0]]['expected']),
        )

        figure = "\\begin{figure}[H]\n"
        figure += "\t\\centering\n"
        figure += "\t\\caption{%s}\n" % caption
        figure += "\t\\label{%s}\n" % label
        figure += "\t\\resizebox{.9\\linewidth}{!}{\n"
        figure += "\t\t\\input{%s}\n" % figure_filename[figure_filename.index("figures"):]
        figure += "\t}\n"
        figure += "\\end{figure}"
        
        with open("%s/%s.tex" % (outputdir, label.replace(':', '_')), 'w') as f:
            f.write(figure)


def generating_vow_table_1(aes_ni=True, inputdir="./raw_data", outputdir="../../../paper/tables"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/vow_table_1" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    # for 20 cpus, which is common to all experiments
    
    if aes_ni:
        filename = "atomkohle_aes_xof_reproducing_table_2_Adj_et_al_gen_full_atk_False_hag_False"
        machine = "solardiesel"
        label = "table:vow-table-1-aes-ni"
        xof = "AES-based"
    else:
        raise ValueError("xxHash results not parsed yet")
    filename = "%s/%s" % (inputdir, filename)


    data = parse(filename=filename)
    state_lengths = sorted(data.keys())
    valid_mem_bits = [2,4,6,8,10,12,14,16]
    
    cpus = data[state_lengths[0]][data[state_lengths[0]].keys()[0]].keys()[0]
    iterations = data[state_lengths[0]][data[state_lengths[0]].keys()[0]][cpus]["total_number"]

    caption = "Reproduction of~\\cite[Table~1]{vOW}, using the %s XOF on \\%s. The experiments are averaged over %d function versions and run with %d cores." % (
        xof,
        machine,
        iterations,
        cpus
    )

    table = "\\begin{table}\n"
    table += "\t\\caption{%s}\n\t\\centering\n" % caption
    table += "\t\\label{%s}\n" % label
    table += "\t\\renewcommand{\\tabcolsep}{0.05in}\n"
    table += "\t\\renewcommand{\\arraystretch}{1.3}\n"
    table += "\t\\begin{tabular}{l" + ("r" * len(valid_mem_bits)) + "}\n"
    table += "\t\t\\toprule\n"
    table += "\t\t & \\multicolumn{%d}{c}{ $\\log{w}$ } \\\\ \cline{2-%d}\n" % (len(valid_mem_bits), len(valid_mem_bits) + 1)
    table += "\t\t $\log{|S|}$ " + "".join([ "& $%s$ " % str(w) for w in sorted(list(valid_mem_bits)) ]) + "\\\\ \\midrule\n"

    for nbits_state in state_lengths:
        table += "\t\t$%d$ " % nbits_state 
        for mem_bits in sorted(list(valid_mem_bits)):
            if mem_bits in data[nbits_state]:
                N = 2 ** nbits_state
                W = 2 ** mem_bits
                FN = 0.45 * N / W
                table += "& $%.2f$ " % (data[nbits_state][mem_bits][cpus]["num_steps"] / (sqrt(N ** 3 / W) / FN))
            else:
                table += "& --- "
        table += "\\\\\n"

    table += "\t\t\\bottomrule\n\t\\end{tabular}\n\\end{table}\n"

    with open("%s/%s.tex" % (outputdir, label.replace(':', '_')), 'w') as f:
        f.write(table)


def generating_gen_vs_cpus_full(inputdir="./raw_data", outputdir="../../../paper/figures"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/gen_vs_cpus_full" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    def stat_vs_cpus(
        experiments,
        figure_filename,
        state_bits=0,
        mem_bits=0,
        statistic="", # selector(sorted([stat for stat in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]].keys() if (stat != "expected" and stat != "full_data")]), buttons=True),
        box_plots=True,
        y_min="function min", # or 0 
        inverse=False,
        expected=True,
        keep_outliers=True,
        ylim=None
    ): 
        if state_bits not in experiments:
            raise ValueError("state_bits %d not available" % state_bits)
        
        if mem_bits not in experiments[state_bits]:
            raise ValueError("mem_bits %d not available" % mem_bits)
        
        inv_exp = -1. if inverse else 1.
        
        results = experiments[state_bits][mem_bits]
        
        if y_min != 0:
            y_min = None
            
        if box_plots:
            import platform
            if platform.system() != 'Linux':
                raise ValueError("box plots only available on Linux")

            if statistic not in results[results.keys()[0]]["full_data"][0]:
                raise ValueError("box plot not available for %s" % statistic)

            # import pdb
            # if statistic == u'collisions':
            #     pdb.set_trace()

            transpose_list = lambda l: map(list, zip(*l))
                        
            fig, ax = plt.subplots()

            # format y axis
            ax.yaxis.set_major_formatter(axis_formatter) 

            # plot average
            ax.plot(range(1, len(results.keys())+1), [results[cpus][statistic] ** inv_exp for cpus in sorted(results.keys())], 'y', linewidth=2)
            ax.legend(['average'+ (" inverse" if inverse else "")])

            # plot expected
            if statistic in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]]["expected"] and expected:
                ax.plot(range(1, len(results.keys())+1), [results[cpus]["expected"][statistic] ** inv_exp for cpus in sorted(results.keys())], 'g', linewidth=2)
                ax.legend(['average'+ (" inverse" if inverse else ""), 'expected' + (" inverse" if inverse else "")])

            # plot boxplot
            data = [ [full_data[statistic] ** inv_exp for full_data in results[cpus]["full_data"] ] for cpus in sorted(results.keys())]
            df = pandas.DataFrame(transpose_list(data), columns=sorted(results.keys())).astype(float)
            df.plot.box(grid=True, ax=ax, showfliers=keep_outliers, ylim=ylim)
            if keep_outliers:
                df.plot.box(grid=True, ylim=(y_min if y_min is not None else ylim if ylim is not None else df.min().min() * .95, df.max().max() * 1.05), ax=ax, showfliers=keep_outliers)
            ax.set_xlabel("$m =$ cores")
            ax.set_ylabel(("${\\textrm{" if inverse else "") + statistic.replace("_", " ") + ("}}^{-1}$" if inverse else ""))
            plt.savefig(figure_filename)
            plt.close()
        else:
            f = [(cpus, results[cpus][statistic] ** inv_exp) for cpus in sorted(results.keys())]
            g = line(f, legend_label=str("measured "+ ("inverse " if inverse else "")+statistic))
            try:
                if statistic in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]]["expected"] and expected:
                    e_f = [(cpus, results[cpus]["expected"][statistic] ** inv_exp) for cpus in sorted(results.keys())]
                    g += line(e_f, rgbcolor=(1, 0, 0), legend_label=str("expected "+ ("inverse " if inverse else "")+statistic))
            except:
                pass
            show(g, axes_labels=['$max crumbs$', statistic], ymin=y_min)

    filename = "%s/azure_some_gen_experiments_gen_full_atk_True_hag_False" % inputdir
    machine = "plongaVMone"
    xof = "xxHash-based"

    experiments = parse(filename=filename)

    # import pdb; pdb.set_trace()

    for nbits_state in experiments:
        for mem_bits in experiments[nbits_state]:
            cpus = sorted(experiments[nbits_state][mem_bits].keys())
            iterations = len(experiments[nbits_state][mem_bits][cpus[0]]['full_data'])

            # add expected wall_time/cycles to the set
            base_cycles = experiments[nbits_state][mem_bits][cpus[0]]['cycles'] * float(cpus[0])
            base_wall_time = experiments[nbits_state][mem_bits][cpus[0]]['wall_time'] * float(cpus[0])
            for threads in cpus:
                experiments[nbits_state][mem_bits][threads]['expected']['cycles'] = base_cycles / float(threads)
                experiments[nbits_state][mem_bits][threads]['expected']['wall_time'] = base_wall_time / float(threads)

            for statistic in [u'mem_collisions', u'dist_points', u'num_steps_locate', u'collisions', u'num_steps', u'dist_cols', u'wall_time', u'wall_time_inverse', u'num_steps_collect', u'cycles', u'cycles_inverse']:
                label = "fig:gen-n-%d-w-%d-%s-stakhanov-full-%s" % (nbits_state, mem_bits, statistic, xof)
                caption = "Box plot for %s as a function of the number of cores used for the full attack, averaged over %s runs of the attack on \\%s. %s random function with $\\log{|S|} = %d$, $\\log{w} = %d$." % (
                    statistic.replace("_", " "), iterations, machine, xof, nbits_state, mem_bits
                )

                if statistic.replace('_inverse', '') in ["wall_time", "cycles"]:
                    caption += " The expected value was computed by picking the average value on the lowest number of cores, and scaling it by the appropriate number for the other number of cores."

                # pdb.set_trace()
                figure_filename = "%s/%s.pgf" % (outputdir, label.replace(':', '_'))
                stat_vs_cpus(
                    experiments,
                    figure_filename,
                    state_bits=nbits_state,
                    mem_bits=mem_bits,
                    statistic=statistic.replace('_inverse', ''),
                    box_plots=True,
                    y_min="function min", # or 0 
                    inverse=bool("_inverse" in statistic),
                    expected=bool(statistic.replace('_inverse', '') in experiments[nbits_state][mem_bits][cpus[0]]['expected']), # and statistic != "dist_cols",
                    keep_outliers=False, # bool(statistic.replace('_inverse', '') not in ["wall_time", "cycles"]),
                    ylim=None, #((8750, 9750) if statistic == "dist_cols" else (81500, 82500) if statistic == "dist_points" else None
                )

                figure = "\\begin{figure}[H]\n"
                figure += "\t\\centering\n"
                figure += "\t\\caption{%s}\n" % caption
                figure += "\t\\label{%s}\n" % label
                figure += "\t\\resizebox{.9\\linewidth}{!}{\n"
                figure += "\t\t\\input{%s}\n" % figure_filename[figure_filename.index("figures"):]
                figure += "\t}\n"
                figure += "\\end{figure}"

                with open("%s/%s.tex" % (outputdir, label.replace(':', '_')), 'w') as f:
                    f.write(figure)


def generating_gen_vs_cpus(aes_ni=True, inputdir="./raw_data", outputdir="../../../paper/figures"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/gen_vs_cpus" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    def stat_vs_cpus(
        experiments,
        figure_filename,
        state_bits=0,
        mem_bits=0,
        statistic="", # selector(sorted([stat for stat in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]].keys() if (stat != "expected" and stat != "full_data")]), buttons=True),
        box_plots=True,
        y_min="function min", # or 0 
        inverse=False,
        expected=True,
        keep_outliers=True,
        ylim=None
    ): 
        if state_bits not in experiments:
            raise ValueError("state_bits %d not available" % state_bits)
        
        if mem_bits not in experiments[state_bits]:
            raise ValueError("mem_bits %d not available" % mem_bits)
        
        inv_exp = -1. if inverse else 1.
        
        results = experiments[state_bits][mem_bits]
        
        if y_min != 0:
            y_min = None
            
        if box_plots:
            import platform
            if platform.system() != 'Linux':
                raise ValueError("box plots only available on Linux")

            if statistic not in results[results.keys()[0]]["full_data"][0]:
                raise ValueError("box plot not available for %s" % statistic)

            transpose_list = lambda l: map(list, zip(*l))
                        
            fig, ax = plt.subplots()

            # format y axis
            ax.yaxis.set_major_formatter(axis_formatter) 
            
            # plot average
            ax.plot(range(1, len(results.keys())+1), [results[cpus][statistic] ** inv_exp for cpus in sorted(results.keys())], 'y', linewidth=2)
            ax.legend(['average'+ (" inverse" if inverse else "")])
            
            # plot expected
            if statistic in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]]["expected"] and expected:
                ax.plot(range(1, len(results.keys())+1), [results[cpus]["expected"][statistic] ** inv_exp for cpus in sorted(results.keys())], 'g', linewidth=2)
                ax.legend(['average'+ (" inverse" if inverse else ""), 'expected' + (" inverse" if inverse else "")])
            
            # plot boxplot
            data = [ [full_data[statistic] ** inv_exp for full_data in results[cpus]["full_data"] ] for cpus in sorted(results.keys())]
            df = pandas.DataFrame(transpose_list(data), columns=sorted(results.keys())).astype(float)
            df.plot.box(grid=True, ax=ax, showfliers=keep_outliers, ylim=ylim)
            if keep_outliers:
                df.plot.box(grid=True, ylim=(y_min if y_min is not None else ylim if ylim is not None else df.min().min() * .95, df.max().max() * 1.05), ax=ax, showfliers=keep_outliers)
            ax.set_xlabel("$m =$ cores")
            ax.set_ylabel(("${\\textrm{" if inverse else "") + statistic.replace("_", " ") + ("}}^{-1}$" if inverse else ""))
            plt.savefig(figure_filename)
            plt.close()
        else:
            f = [(cpus, results[cpus][statistic] ** inv_exp) for cpus in sorted(results.keys())]
            g = line(f, legend_label=str("measured "+ ("inverse " if inverse else "")+statistic))
            try:
                if statistic in experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]][experiments[experiments.keys()[0]][experiments[experiments.keys()[0]].keys()[0]].keys()[0]]["expected"] and expected:
                    e_f = [(cpus, results[cpus]["expected"][statistic] ** inv_exp) for cpus in sorted(results.keys())]
                    g += line(e_f, rgbcolor=(1, 0, 0), legend_label=str("expected "+ ("inverse " if inverse else "")+statistic))
            except:
                pass
            show(g, axes_labels=['$max crumbs$', statistic], ymin=y_min)
    
    if aes_ni:
        filename = "some_large_gen_experiments_aes_ni_xof_gen_full_atk_False_hag_False"
        machine = "atomkohle"
        xof = "AES-based"
    else:
        raise ValueError("have to merge the files first")
    filename = "%s/%s" % (inputdir, filename)

    experiments = parse(filename=filename)

    nbits_state = max(experiments.keys())
    mem_bits = max(experiments[nbits_state].keys())
    cpus = sorted(experiments[nbits_state][mem_bits].keys())
    iterations = experiments[nbits_state][mem_bits][cpus[0]]['total_number']

    # add expected wall_time/cycles to the set
    base_cycles = experiments[nbits_state][mem_bits][cpus[0]]['cycles'] * float(cpus[0])
    base_wall_time = experiments[nbits_state][mem_bits][cpus[0]]['wall_time'] * float(cpus[0])
    for threads in cpus:
        experiments[nbits_state][mem_bits][threads]['expected']['cycles'] = base_cycles / float(threads)
        experiments[nbits_state][mem_bits][threads]['expected']['wall_time'] = base_wall_time / float(threads)

    for statistic in [u'mem_collisions', u'dist_points', u'num_steps_locate', u'collisions', u'num_steps', u'dist_cols', u'wall_time', u'wall_time_inverse', u'num_steps_collect', u'cycles', u'cycles_inverse']:
        label = "fig:gen-n-%d-w-%d-%s-stakhanov" % (nbits_state, mem_bits, statistic)
        caption = "Box plot for %s as a function of the number of cores used for the attack, averaged over %s function versions on \\%s. %s random function with $\\log{|S|} = %d$, $\\log{w} = %d$." % (
            statistic.replace("_", " "), iterations, machine, xof, nbits_state, mem_bits
        )

        if statistic.replace('_inverse', '') in ["wall_time", "cycles"]:
            caption += " The expected value was computed by picking the average value on the lowest number of cores, and scaling it by the appropriate number for the other number of cores."

        figure_filename = "%s/%s.pgf" % (outputdir, label.replace(':', '_'))
        stat_vs_cpus(
            experiments,
            figure_filename,
            state_bits=nbits_state,
            mem_bits=mem_bits,
            statistic=statistic.replace('_inverse', ''),
            box_plots=True,
            y_min="function min", # or 0 
            inverse=bool("_inverse" in statistic),
            expected=bool(statistic.replace('_inverse', '') in experiments[nbits_state][mem_bits][cpus[0]]['expected']), # and statistic != "dist_cols",
            keep_outliers=False, # bool(statistic.replace('_inverse', '') not in ["wall_time", "cycles"]),
            ylim=((8750, 9750) if statistic == "dist_cols" else (81500, 82500) if statistic == "dist_points" else None)
        )

        figure = "\\begin{figure}[H]\n"
        figure += "\t\\centering\n"
        figure += "\t\\caption{%s}\n" % caption
        figure += "\t\\label{%s}\n" % label
        figure += "\t\\resizebox{.9\\linewidth}{!}{\n"
        figure += "\t\t\\input{%s}\n" % figure_filename[figure_filename.index("figures"):]
        figure += "\t}\n"
        figure += "\\end{figure}"
        
        with open("%s/%s.tex" % (outputdir, label.replace(':', '_')), 'w') as f:
            f.write(figure)


def generating_precomputation_plots(inputdir="./raw_data", outputdir="../../../paper/figures"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/precomputation_plots" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    filenames = [
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_32_20" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_36_22" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_40_25" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_44_27" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_48_30" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_52_33" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_56_35" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_60_38" % inputdir,
    ]
    machine = "atomkohle"

    for filename in filenames:
        caption = "" # placeholder
        figure = "" # placeholder

        lines = []

        with open(filename) as f:
            lines = f.readlines()

        pds = []
        total_time = []
        wall_time = []
        precomp_time = []
        cycles = []
        figure_filename = ""

        for line in lines:
            data = json.loads(line)
            modulus, pd = data['k']
            results = data['v']
            label = "fig:precomp_%s" % modulus
            caption = "Effect of precomputation on running time of vOW, for SIKE with $e_2 = %s$. Experiments run on \\%s." % (modulus.split('_')[1], machine)


            pds.append(pd)
            total_time.append(results["total_time"])
            wall_time.append(results["wall_time"])
            precomp_time.append(results["precomp_time"])
            cycles.append(results["cycles"])

        # total time
        plot = plt.plot(pds, total_time)
        ax = plot[0].axes
        ax.yaxis.set_major_formatter(axis_formatter) 
        # import pdb; pdb.set_trace()
        for item in ([ax.title, ax.xaxis.label, ax.yaxis.label, ax.yaxis.offsetText] + ax.get_xticklabels() + ax.get_yticklabels()):
            item.set_fontsize(76)
        plt.xlabel('precomputation depth')
        plt.ylabel("total time")
        plt.grid()
        plt.tight_layout()
        plt.savefig(outputdir + "/" + label.replace(':', '_') + "_total_time.pgf")
        plt.close()

        # wall time
        plot = plt.plot(pds, wall_time)
        ax = plot[0].axes
        ax.yaxis.set_major_formatter(axis_formatter) 
        for item in ([ax.title, ax.xaxis.label, ax.yaxis.label, ax.yaxis.offsetText] + ax.get_xticklabels() + ax.get_yticklabels()):
            item.set_fontsize(76)
        plt.xlabel('precomputation depth')
        plt.ylabel("wall time")
        plt.grid()
        plt.tight_layout()
        plt.savefig(outputdir + "/" + label.replace(':', '_') + "_wall_time.pgf")
        plt.close()

        # precomp time
        plot = plt.plot(pds, precomp_time)
        ax = plot[0].axes
        ax.yaxis.set_major_formatter(axis_formatter) 
        for item in ([ax.title, ax.xaxis.label, ax.yaxis.label, ax.yaxis.offsetText] + ax.get_xticklabels() + ax.get_yticklabels()):
            item.set_fontsize(76)
        plt.xlabel('precomputation depth')
        plt.ylabel("precomputation time")
        plt.grid()
        plt.tight_layout()
        plt.savefig(outputdir + "/" + label.replace(':', '_') + "_precomp_time.pgf")
        plt.close()

        # total time
        plot = plt.plot(pds, cycles)
        ax = plot[0].axes
        ax.yaxis.set_major_formatter(axis_formatter) 
        for item in ([ax.title, ax.xaxis.label, ax.yaxis.label, ax.yaxis.offsetText] + ax.get_xticklabels() + ax.get_yticklabels()):
            item.set_fontsize(76)
        plt.xlabel('precomputation depth')
        plt.ylabel("cycles")
        plt.grid()
        plt.tight_layout()
        plt.savefig(outputdir + "/" + label.replace(':', '_') + "_cycles.pgf")
        plt.close()

        figure = "\\begin{figure}{}\n"
        figure += "\t\\caption{%s}\n" % caption
        figure += "\t\\label{%s}\n" % label
        figure += "\t\\centering\n"

        for sub in [ "precomp_time", "cycles", "wall_time", "total_time"]:
            figure += "\t\\begin{subfigure}{.45\\linewidth}\n"
            figure += "\t\t\\centering\n"
            figure += "\t\t\\resizebox{\\textwidth}{!}{\n"
            figure += "\t\t\\input{figures/fig_precomp_%s_%s.pgf}\n" % (modulus, sub)
            figure += "\t\t}\n"
            figure += "\t\\end{subfigure}\n"

        figure += "\\end{figure}\n"

        with open(outputdir + "/" + label.replace(':', '_') + ".tex", 'w') as f:
            f.write(figure)


def generating_precomputation_table(inputdir="./raw_data", outputdir="../../../paper/tables"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/precomputation_table" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    filenames = [
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_32_20" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_36_22" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_40_25" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_44_27" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_48_30" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_52_33" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_56_35" % inputdir,
        "%s/atomkohle_precomputation_sike_sike_bench_Update_p_60_38" % inputdir,
    ]
    machine = "atomkohle"

    experiments = {}

    for filename in filenames:
        caption = "" # placeholder
        figure = "" # placeholder

        lines = []

        with open(filename) as f:
            lines = f.readlines()

        # pds = []
        # total_time = []
        # wall_time = []
        # precomp_time = []
        # cycles = []
        # figure_filename = ""

        for line in lines:
            data = json.loads(line)
            modulus, pd = data['k']
            results = data['v']

            if modulus not in experiments:
                experiments[modulus] = {}
            
            experiments[modulus][pd] = results
    label = "table:precomputation"
    caption = "Effect of precomputation on the running time of the SIKE step function. Numbers represent the cumulative running time in seconds of %d calls to the step function, for the corresponding modulus and precomputation depth $\Delta$. All experiments were run on \\%s." % (experiments[experiments.keys()[0]][0]['total_number'], machine)

    largest_modulo = max(experiments.keys())
    max_pds = sorted([pd for pd in experiments[largest_modulo].keys()])

    table = "\\begin{table}\n"
    table += "\t\\caption{%s}\n\t\\centering\n" % caption
    table += "\t\\label{%s}\n" % label
    table += "\t\\renewcommand{\\tabcolsep}{0.05in}\n"
    table += "\t\\renewcommand{\\arraystretch}{1.3}\n"
    table += "\t\\begin{tabular}{l" + ("r" * len(max_pds)) + "}\n"
    table += "\t\t\\toprule\n"
    table += "\t\t & \\multicolumn{%d}{c}{ $\Delta$ } \\\\ \cline{2-%d}\n" % (len(max_pds), len(max_pds) + 1)
    table += "\t\t $e_2$ " + "".join([ "& $%s$ " % str(w) for w in sorted(list(max_pds)) ]) + "\\\\ \\midrule\n"

    for modulus in sorted(experiments.keys()):
        table += "\t\t$%s$ " % modulus.split('_')[1]
        for pd in sorted(list(max_pds)):
            if pd in experiments[modulus]:
                table += "& $%.2f$ " % (experiments[modulus][pd]['total_time'])
            else:
                table += "& --- "
        table += "\\\\\n"

    table += "\t\t\\bottomrule\n\t\\end{tabular}\n\\end{table}\n"

    with open("%s/%s.tex" % (outputdir, label.replace(':', '_')), 'w') as f:
        f.write(table)


def main():
    print("generating Adj et al's table 2")
    generating_adj_table_2(outputdir="./tables", aes_ni=True)
    generating_adj_table_2(outputdir="./tables", aes_ni=False)
    print("generating Adj et al's table 3")
    generating_adj_table_3('sike', outputdir="./tables")
    generating_adj_table_3('sidh', outputdir="./tables")
    generating_adj_table_3_gen(aes_ni=True, outputdir="./tables")
    generating_adj_table_3_gen(aes_ni=False, outputdir="./tables")
    print("generating crumb plots")
    generating_crumb_plots(aes_ni=True, outputdir="./figures")
    print("generating sike vs cpus plots")
    generating_sike_vs_cpus('windowed', outputdir="./figures")
    generating_sike_vs_cpus('stakhanov', outputdir="./figures")
    generating_sike_vs_cpus('nobiggie', outputdir="./figures")
    print("generating vOW table 1 plots")
    generating_vow_table_1(aes_ni=True, outputdir="./tables")
    print("generating gen vs cpus plots")
    generating_gen_vs_cpus(aes_ni=True, outputdir="./figures")
    generating_gen_vs_cpus_full(outputdir="./figures")
    print("generating precomputation plots")
    generating_precomputation_plots(outputdir="./figures")
    print("generating precomputation table")
    generating_precomputation_table(outputdir="./tables")


import __main__
if hasattr(__main__, "__file__") and __name__ == "__main__":
    main()
