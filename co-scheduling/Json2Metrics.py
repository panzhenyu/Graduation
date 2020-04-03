# note that the json file must be produced by MergeOut.py
# this python script will add cpi, ipc, fairness into json

import json, sys
import numpy as np
from utils import loadSingle
from config.path import CO_SCHEDULING_HOME

def loadFromJson(json_file):
    fp = open(json_file, "r")
    s = fp.read()
    data = json.loads(s)
    fp.close()
    return data

# calculate CPI, IPC, Fairness
# note that a taskSet's cycle must be a average, not sum of its tasks
def calcMetrics(data, single):
    for scheduleObj in data:
        for taskSets in scheduleObj.values():
            # calc CPI and IPC for schedule
            IPC_sum, CPI_sum = 0, 0
            slowdown = []
            # calc Fairness for schedule
            for taskSet in taskSets.values():
                for taskName in taskSet.keys():
                    taskAttr = taskSet[taskName]
                    # get attribute from task
                    # access = taskAttr['access']
                    instructions = taskAttr['instructions']
                    cycle = taskAttr['cycle']
                    # miss = taskAttr['miss']
                    taskCPI = float(cycle) / instructions
                    taskIPC = float(instructions) / cycle
                    taskAttr['IPC'] = taskIPC
                    taskAttr['CPI'] = taskCPI

                    # collect task's ipc and cpi
                    IPC_sum += taskIPC
                    CPI_sum += taskCPI
                    # calc slowdown for Fairness
                    singleInstructions, singleCycle = single[taskName]['instructions'], single[taskName]['cycle']
                    singleCPI = float(singleCycle) / singleInstructions
                    slow = float(taskCPI) / singleCPI
                    slowdown.append(slow)
            # used for get pure metrics
            # keys = []
            # for key in taskSets.keys():
            #     keys.append(key)
            # for key in keys:
            #     del taskSets[key]
            taskSets['CPI'] = CPI_sum
            taskSets['IPC'] = IPC_sum
            taskSets['Unfairness'] = np.std(slowdown, ddof=1) / np.mean(slowdown)

def save(output_file, data):
    fp = open(output_file, "w")
    s = json.dumps(data, indent=4)
    fp.write(s)
    fp.close()

if __name__ == "__main__":
    SINGLE = CO_SCHEDULING_HOME + "/data/corun_result/single"
    argc = len(sys.argv)
    if argc != 3:
        print("usage: python3 Json2Metrics.py json_file output_file")
        sys.exit(-1)
    json_file = sys.argv[1]
    output_file = sys.argv[2]
    
    data = loadFromJson(json_file)
    single = loadSingle(SINGLE)
    calcMetrics(data, single)
    
    save(output_file, data)
