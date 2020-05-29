from __future__ import print_function
import json
from sage.all import save


def parse(filename="full_atk_False_hag_True"):
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


def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-server", type=str, default="", help="server name (eg solardiesel_)")
    parser.add_argument("-file", type=str, default="", help="filename")
    args = parser.parse_args()

    if args.file != "":
        stats = parse(filename=args.file)
        save(stats, "%s.sobj" % args.file)
        return

    # gen
    for full in [True, False]:
        for hag in [True, False]:
            try:
                stats = parse(filename="./%sgen_full_atk_%s_hag_%s" % (args.server, full, hag))
                save(stats, "./%sgen_full_atk_%s_hag_%s.sobj" % (args.server, full, hag))
            except:
                print("%sgen_full_atk_%s_hag_%s not available" % (args.server, full, hag))

    # sidh
    for full in [True, False]:
        for hag in [True, False]:
            for precomp in [True, False]:
                try:
                    stats = parse(filename="./%ssidh_full_atk_%s_hag_%s_precomp_%s" % (args.server, full, hag, precomp))
                    save(stats, "./%ssidh_full_atk_%s_hag_%s_precomp_%s.sobj" % (args.server, full, hag, precomp))
                except:
                    print("%ssidh_full_atk_%s_hag_%s_precomp_%s not available" % (args.server, full, hag, precomp))

    # sike
    for full in [True, False]:
        for hag in [True, False]:
            for precomp in [20]:
                try:
                    stats = parse(filename="./%ssike_full_atk_%s_hag_%s_precomp_%s" % (args.server, full, hag, precomp))
                    save(stats, "./%ssike_full_atk_%s_hag_%s_precomp_%s.sobj" % (args.server, full, hag, precomp))
                except:
                    print("%ssike_full_atk_%s_hag_%s_precomp_%s not available" % (args.server, full, hag, precomp))


import __main__
if hasattr(__main__, "__file__") and __name__ == "__main__":
    main()
