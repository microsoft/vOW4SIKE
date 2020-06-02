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
            # since the file may be generated in different chunks
            # (due to process being killed at points)
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
