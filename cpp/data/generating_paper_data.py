from __future__ import print_function
import os, sys, os.path
from parsing_results import parse
from sage.all import mean, median, RR


def generating_adj_table_2(inputdir="./raw_data", outputdir="../../../paper/tables", aes_ni=True):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/adj_table_2" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    if aes_ni:
        filename = "atomkohle_reproducing_table_2_Adj_et_al_using_gen_aes_ni_xof_gen_full_atk_False_hag_False"
        machine = "atomkohle"
        label = "table:adj-table-2-aes-ni"
        xof = "AES-based"
    filename = "%s/%s" % (inputdir, filename)


    data = parse(filename=filename)
    state_lengths = data.keys()
    mem_lengths = data[state_lengths[0]].keys()
    cpus = data[state_lengths[0]][mem_lengths[0]].keys()[0]
    iterations = data[state_lengths[0]][mem_lengths[0]][cpus]["total_number"]

    caption = "Reproduction of Table 2 from~\\cite{adjetal}, using the %s XOF on \\%s. Experiments are averaged over %d function versions using %d cores." % (
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


def generating_adj_table_3_gen(aes_ni=True, inputdir="./raw_data", outputdir="../../../paper/tables"):
    # First generate an output subdirectory
    subdir = os.path.abspath("%s/adj_table_3" % outputdir)
    if not os.path.exists(subdir):
        os.makedirs(subdir)
    outputdir = subdir

    if aes_ni:
        filename = "atomkohle_reproducing_table_3_Adj_et_al_using_gen_aes_ni_xof_gen_full_atk_True_hag_False"
        machine = "atomkohle"
        xof = "AES-based"
        label = "table:adj-table-3-aes-ni"
    filename = "%s/%s" % (inputdir, filename)

    table = "\\begin{table}\n"

    # how many cpus?
    data = parse(filename=filename)
    state_lengths = sorted(data.keys())
    cpus = data[state_lengths[0]][data[state_lengths[0]].keys()[0]].keys()[0]

    caption = "Reproduction of Table 3 from~\\cite{adjetal}, using our a %s generic random function on \\%s. Experiments are run using %d cores." % (
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


def main():
    print("generating Adj et al's table 2")
    generating_adj_table_2(outputdir="./tables", aes_ni=True)
    print("generating Adj et al's table 3")
    generating_adj_table_3_gen(aes_ni=True, outputdir="./tables")


import __main__
if hasattr(__main__, "__file__") and __name__ == "__main__":
    main()
