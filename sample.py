#!/usr/bin/env python3

import argparse
import os.path as path
import random

import test_config

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--infiles", nargs=3, required=True)
    parser.add_argument("--samples", type=int, required=True)

    args = parser.parse_args()
    
    return args


def create_template(filename, samples):
    with open(filename, "r") as f:
        template = [False for i in range(len(f.readlines())//2)]

    for i in range(samples//2):
        template[i] = True

    random.shuffle(template)

    return template


def sample(filename, template):
    with open(filename, "r") as f:
        lines = f.readlines()

    clean = [line for line in lines if line.split()[0] == "1"]
    print(len(clean))
    clean = [c for c,t in zip(clean, template) if t==True]
    print(len(clean))
    dirty = [line for line in lines if line.split()[0] == "-1"]
    print(len(dirty))
    dirty = [d for d,t in zip(dirty, template) if t == True]
    print(len(dirty))
    reduced = clean + dirty

    return reduced


def write_samples(samples, filename):
    with open(filename, "w") as f:
        f.write(''.join(samples))

def main():
    args = parse_args()

    normfile = args.infiles[0]
    highfile = args.infiles[1]
    lowfile = args.infiles[2]

    norm_test_answers = test_config.strip_test_data(normfile)
    high_test_answers = test_config.strip_test_data(highfile)
    low_test_answers = test_config.strip_test_data(lowfile)
    answers = test_config.combine_answers(norm_test_answers, high_test_answers, low_test_answers)
    

    template = create_template(normfile, args.samples)
    print("norm")
    norm = sample(normfile, template)
    print("\nhigh")
    high = sample(highfile, template)
    print("\nlow")
    low = sample(lowfile, template)

    write_samples(norm, normfile+".samples")
    write_samples(high, highfile+".samples")
    write_samples(low, lowfile+".samples")

if __name__ == "__main__":
    main()
