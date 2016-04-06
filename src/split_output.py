#!/usr/bin/env python3

import argparse
import fileinput
import sys
import os.path as path

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("basename", type=str)

    args = parser.parse_args()

    return args.basename


CLEAN = 0
DIRTY = 1
AVG = 0
HI = 1
LO = 2

def new_state(old_state, line):
    if (line.startswith("CLEAN:")):
        return (CLEAN, old_state[1])
    if (line.startswith("DIRTY:")):
        return (DIRTY, old_state[1])
    if (line.startswith("AVERAGE:")):
        return (old_state[0], AVG)
    if (line.startswith("MULHI:")):
        return (old_state[0], HI)
    if (line.startswith("MULLO:")):
        return (old_state[0], LO)
    print("Unknown input label: {}".format(line))
    sys.exit(-1)


def split_input():
    state = (None, None)
    clean_vecs = (list(), list(), list())
    dirty_vecs = (list(), list(), list())
    vecs = (clean_vecs, dirty_vecs);

    for line in sys.stdin:
        line = line.strip()
        if (line == ''):
            continue
        if (line.isupper()):
            state = new_state(state, line)
            continue
        line = ' '.join(['{}:{}'.format(i+1, ent) for i,ent in enumerate(line.split())])
        vecs[state[0]][state[1]].append("{} {}".format((-1)**state[0], line))
        
    return vecs


def write_file(filename, data):
    full_filename = path.abspath(filename)
    with open(full_filename, 'w') as f:
        f.write('\n'.join(data))
        f.write('\n')

TYPE = ["average", "mulhi", "mullo"]

def output_vectors(basename, vecs):
    for i in range(3):
        filename = "{}_{}_vectors.txt".format(basename, TYPE[i])
        write_file(filename, vecs[0][i]+vecs[1][i])

def main():
    basename = parse_args()

    vecs = split_input()
    output_vectors(basename, vecs)


if __name__ == "__main__":
    main()
