# merge output data
# usage: python3 MergeOutput.py output_file
# the data file must named task_set_combination and PARALLEL_OUT

from config.path import AUTOTEST_COMBINATION, AUTOTEST_CORUNOUT
from utils import loadCorunResult
import sys, json

# note that the schedule result file insist of a head line with results, the head line contains a series of algorithm name separated in ' '
# schedule result is list format:
# schedule_result = [scheduleObj, scheduleObj, ...]
# scheduleObj = {algorithmName: taskSets, ...}
# taskSets = {taskNameSeq: taskSet, ...}
# taskSet = {taskName: taskAttr, ...}
# taskAttr = {cycle: cycle_num, instructions: instruction_num, miss: miss_num, access: access_num}
def loadSchedResult(filename):
    fp = open(AUTOTEST_COMBINATION, "r")

    sched_result, scheduleObj, taskSets = [], {}, {}
    algorithmName, nameSelector = fp.readline().strip().split(' '), 0

    for line in fp.readlines():
        line = line.strip()
        if len(line) <= 0:
            continue
        tag = line[0:3]
        if tag == "seq" or tag == "---":
            if len(taskSets) != 0:
                algorithm = algorithmName[nameSelector]
                scheduleObj[algorithm] = taskSets
            nameSelector += 1
            taskSets = {}
            if tag == "seq":
                if len(scheduleObj) != 0:
                    sched_result.append(scheduleObj)
                nameSelector = 0
                scheduleObj = {}
        else:
            names = line.split(' ')
            names.sort()
            taskNameSeq = ""
            for name in names:
                taskNameSeq += name + " "
            taskNameSeq = taskNameSeq[0:-1]
            taskSets[taskNameSeq] = None
    if len(scheduleObj) != 0:
        sched_result.append(scheduleObj)

    fp.close()
    return sched_result

# fill the sched_result with corun_result
# mainly fill the taskAttr in sched_result
def merge(sched_result, corun_result):
    for scheduleObj in sched_result:
        for taskSets in scheduleObj.values():
            for taskNameSeq in taskSets.keys():
                if taskNameSeq in corun_result:
                    taskSets[taskNameSeq] = corun_result[taskNameSeq]
                else:
                    # pass
                    print(taskNameSeq, "is not in " + AUTOTEST_CORUNOUT)

def saveMergeResult(filename, merge_result):
    output = open(filename, "w")
    s = json.dumps(merge_result, indent=4)
    output.write(s)
    output.close()

if __name__ == "__main__":
    arg_len = len(sys.argv)
    if arg_len != 2:
        print("usage: python3 MergeOutput.py output_file")
        sys.exit(-1)
    output_file  = sys.argv[1]

    sched_result = loadSchedResult(AUTOTEST_COMBINATION)
    corun_result = loadCorunResult(AUTOTEST_CORUNOUT)
    merge(sched_result, corun_result)
    saveMergeResult(output_file, sched_result)
    print("sched_result length: {0} corun_result length: {1}".format(len(sched_result), len(corun_result)))
