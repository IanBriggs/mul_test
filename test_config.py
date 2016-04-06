#!/usr/bin/env python3

import argparse
import os
import sys
import os.path as path
import subprocess



def parse_args():
    parser = argparse.ArgumentParser()
#    parser.add_argument("--norm-svm-flags", nargs="*", default=list())
#    parser.add_argument("--high-svm-flags", nargs="*", default=list())
#    parser.add_argument("--low-svm-flags", nargs="*", default=list())
#    parser.add_argument("--all-svm-flags", nargs="*", default=list())
    parser.add_argument("--training-sets", nargs=3, required=True)
    parser.add_argument("--testing-sets", nargs=3, required=True) # testing sets must be aligned (line 1 from all files should be from the same place of the same experiment run)
    parser.add_argument("--datafile", required=True)
    args = parser.parse_args()

    # if ((args.all_svm_flags == list()) and (args.norm_svm_flags == list() or 
    #                                       args.norm_svm_flags == list() or 
    #                                       args.norm_svm_flags == list())):
    #     print("If all-svm-flags is not given then norm, high, and low svm flags must be filed")
    #     sys.exit(-1)
        
    return args


def run(cmd, args):
    command = cmd+" "+" ".join(args)
    print(command, "\n")
    with subprocess.Popen(command, shell=True) as proc:
        proc.wait()
        


def run_single_detector(training_set, args, testing_set):
    run("svm-train", args + [training_set])
    run("svm-predict", [testing_set, training_set+".model", training_set+".predict"])
    with open(training_set+".predict", 'r') as f:
        predictions = [int(float(line)) for line in f.readlines()]
    return predictions


def combine_predictions(norm, high, low):
    full_predictions = list()
    for i in range(len(norm)):
        if (-1 in (norm[i], high[i], low[i])):
            fp = -1
        else:
            fp = 1
        full_predictions.append(fp)
    return full_predictions


def prediction_rate(predictions, answers):
    correct = sum([p == a for p,a in zip(predictions, answers)])
    return correct/len(answers)


def combine_answers(norm, high, low):
    answers =list()
    for i in range(len(norm)):
        if (norm[i] != high[i] or high[i] != low[i]):
            print("Testing sets must be aligned")
            print("(line 1 from all files should be from the same place of the same experiment run)")
            sys.exit(-1)
        answers.append(norm[i])

    return answers


def strip_test_data(testfile):
    with open(testfile, "r") as f:
        lines = [line.split() for line in f.readlines() if line.strip() != ""]
    test_answers = [int(line[0]) for line in lines]

    return test_answers

def main():
    args = parse_args()

    norm_test_answers = strip_test_data(args.testing_sets[0])
    high_test_answers = strip_test_data(args.testing_sets[1])
    low_test_answers = strip_test_data(args.testing_sets[2])
    answers = combine_answers(norm_test_answers, high_test_answers, low_test_answers)

    with open(args.datafile, "w") as f:
        f.write("s, t, gamma, coeff0, norm, high, low, combined\n")

    for s in range(3):
        cmd_args = [None for i in range(6)]
        cmd_args[0] = "-s {} ".format(s)
        cmd_args[1] = "-t 1"
        cmd_args[2] = "-c 10000 "
        for d in range(1, 4):
            cmd_args[3] = "-d {} ".format(d)
            for g in [10**p for p in range(-15, -2, 3)]:
                cmd_args[4] = "-g {} ".format(g)
                for r in range(-2,3):
                    cmd_args[5] = "-r {} ".format(r)

                    print("\n\nRunning norm model:")
                    norm_predictions = run_single_detector(args.training_sets[0], cmd_args, args.testing_sets[0])
                    print("\n\nRunning high model:")
                    high_predictions = run_single_detector(args.training_sets[1], cmd_args, args.testing_sets[1])
                    print("\n\nRunning low model:")
                    low_predictions = run_single_detector(args.training_sets[2], cmd_args, args.testing_sets[2])

                    full_predictions = combine_predictions(norm_predictions, high_predictions, low_predictions)

                    nrate = prediction_rate(norm_predictions, answers)
                    hrate = prediction_rate(high_predictions, answers)
                    lrate = prediction_rate(low_predictions, answers)
                    frate = prediction_rate(full_predictions, answers)

                    with open(args.datafile, 'a') as f:
                        f.write("{}, {}, {}, {}, {}, {}, {}, {}\n".format(s, d, g, r, nrate, hrate, lrate, frate))


    



if __name__ == "__main__":
    main()
