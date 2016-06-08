#!/usr/bin/env python3
# Ian Briggs
# U0692013
#


import multiprocessing as MP
import subprocess as SP
import argparse as AP
import re
import time as T
import sys
import os
import os.path as path

from pprint import pprint

output_lock = MP.Lock()
output_file = None
csv_file = "differential_run.csv"


def un_color(text):
    return text.replace('\033[0m','').replace('\033[0;31m','').replace('\033[0;32m','').replace('\033[0;33m','')


def red(text):
    return '\033[0;31m' + text + '\033[0m'

def green(text):
    return '\033[0;32m' + text + '\033[0m'

def yellow(text):
    return '\033[0;33m' + text + '\033[0m'


def parse_args():
    global output_file
    desc = "Schedule long running jobs in a batch-style fashion"
    epi =  "Commands are run in subdiretories of the output directory"         \
           ".stout and stderr of the command are put "                         \
           "into a \"command_output.txt\" file in the subdirectories"
    parser = AP.ArgumentParser(description=desc, epilog=epi)
    parser.add_argument("-p", metavar="procs", type=int, default=None,
                        help="number of concurrent jobs to run, "
                        "default is the number of cores")
    parser.add_argument("-o", metavar="output_dir", type=str, default="gen",
                        help="directory to put output subdirectories into, "
                        "default is \"gen\"")
    parser.add_argument("--training-sets", nargs=3, required=True)
    parser.add_argument("--testing-sets", nargs=3, required=True) # testing sets must be aligned (line 1 from all files should be from the same place of the same experiment run)
    parser.add_argument("--datafile", required=True)

    args = parser.parse_args()
    
    output_file = path.abspath(args.datafile)

    args.training_sets[0] = path.abspath(args.training_sets[0])
    args.training_sets[1] = path.abspath(args.training_sets[1])
    args.training_sets[2] = path.abspath(args.training_sets[2])

    args.testing_sets[0] = path.abspath(args.testing_sets[0])
    args.testing_sets[1] = path.abspath(args.testing_sets[1])
    args.testing_sets[2] = path.abspath(args.testing_sets[2])

    if (args.p == None):
        # figure the number of cores and queue up that many processes
        # this may throw an error, is so catch it and assume one cpu core
        try:
            args.p = MP.cpu_count()
        except ValueError:
            args.p = 1


    return args



def generate_work_queue():
    # queue them up
    work_queue = MP.Queue()
    for s in range(3):
        for d in range(1, 4):
            for g in [10**p for p in range(-2, 3)]:
                for r in range(-2,3,2):
                    work_queue.put((s, d, g, r))
    
    return work_queue


def mkdir(new_dir):
    if os.path.isfile(new_dir):
        return False
    if os.path.isdir(new_dir):
        return True
    try:
        os.mkdir(new_dir)
        return True
    except:
        return False

def run(cmd, args):
    command = cmd+" "+" ".join(args)
    with SP.Popen(command, shell=True, stdout=SP.DEVNULL) as proc:
        proc.wait()
        


def run_single_detector(training_set, args, testing_set):
    model_file = path.split(training_set)[1]+".model"
    predict_file = path.split(training_set)[1]+".predict"
    run("svm-train", args + [training_set, model_file])
    run("svm-predict", [testing_set, model_file, predict_file])
    with open(predict_file, 'r') as f:
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


def run_command(dirnum_lock, dirnum, work_queue, output_dir):
        # get the next command
        s, d, g, r = work_queue.get()

        # get to base
        os.chdir(output_dir)
        
        # get new subdir based on atomic incrmeent
        new_subdir = "s_{}_d_{}_g_{}_r{}".format(s, d, g, r)

        # make and move to new subdir
        mkdir(new_subdir)
        os.chdir(new_subdir)

        # run
        cmd_args = "-s {}#-t 1#-c 10000#-d {}#-g {}#-r {}".format(s, d, g, r).split('#')
        norm_predictions = run_single_detector(TRAINING_SETS[0], cmd_args, TESTING_SETS[0])
        high_predictions = run_single_detector(TRAINING_SETS[1], cmd_args, TESTING_SETS[1])
        low_predictions = run_single_detector(TRAINING_SETS[2], cmd_args, TESTING_SETS[2])

        full_predictions = combine_predictions(norm_predictions, high_predictions, low_predictions)

        nrate = prediction_rate(norm_predictions, ANSWERS)
        hrate = prediction_rate(high_predictions, ANSWERS)
        lrate = prediction_rate(low_predictions, ANSWERS)
        frate = prediction_rate(full_predictions, ANSWERS)

        csv_entry = "{}, {}, {}, {}, {}, {}, {}, {}\n".format(s, d, g, r, nrate, hrate, lrate, frate)

        output_lock.acquire()
        dirnum_lock.acquire()
        this_iter = dirnum.value
        dirnum.value += 1
        dirnum_lock.release()
        print("Done with {}".format(this_iter))
        with open(output_file, 'a') as f:
            f.write(csv_entry)
        output_lock.release()
            

def process_worker(dirnum_lock, dirnum, 
                   work_queue, output_dir):
    while(not work_queue.empty()):
        # run the command
        run_command(dirnum_lock, dirnum, work_queue, output_dir)





ANSWERS=None
TRAINING_SETS=None
TESTING_SETS=None
def main():
    global ANSWERS, TRAINING_SETS, TESTING_SETS
    # parse args and display number of procs used
    args = parse_args()
    print("Using {} concurrent processes".format(args.p))

    norm_test_answers = strip_test_data(args.testing_sets[0])
    high_test_answers = strip_test_data(args.testing_sets[1])
    low_test_answers = strip_test_data(args.testing_sets[2])
    ANSWERS = combine_answers(norm_test_answers, high_test_answers, low_test_answers)
    TRAINING_SETS = args.training_sets
    TESTING_SETS = args.testing_sets

    # generate work queue
    work_queue = generate_work_queue()
    work_amount = work_queue.qsize()
    print("{} commands to execute".format(work_amount))

    # make/check output dir
    if (not mkdir(args.o)):
        print("Error: unable to make/read output directory")
        sys.exit()
    args.o = os.path.abspath(args.o)


    # make process list
    arguments = (MP.Lock(), MP.Value('i', 0), # output directory number
                 work_queue,      # work communication
                 args.o)      # directory to work from
    process_list = [MP.Process(target=process_worker, args=arguments)
                    for i in range(args.p)]
    
    with open(output_file, 'w') as f:
            f.write("s, t, gamma, coeff0, norm, high, low, combined\n")

    # start running
    for pro in process_list:
        pro.start()

    # first output regardless
    #sys.stdout.write("Finished with {} out of {}\r".format(0, work_amount))

    # print to stdout the current number of commands processed
    # while(progress.value < work_amount):
    #     parent_conn.recv() # wait on recv
    #     progress_lock.acquire()
    #     #sys.stdout.write("Finished with {} out of {}\r".format(progress.value,
    #     #                                                       work_amount))
    #     progress_lock.release()
    #     sys.stdout.flush()

    # final output so next print doesn't overwite progress info
    #print("Finished with {} out of {}".format(progress.value, work_amount))

    # make sure we don't leave stragglers
    #    print("cleaning up")
    for pro in process_list:
        pro.join()

    print("Done!")


if __name__ == "__main__":
    main()

