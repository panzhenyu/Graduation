# note that the json file must be produced by MergeOut.py
# this python script will add cpi, ipc, fairness into json

import json, sys
import numpy as np

def loadFromJson(json_file):
    fp = open(json_file, "r")
    s = fp.read()
    data = json.loads(s)
    fp.close()
    return data

# single data format:
# single data  = {taskName: taskAttr}
# taskAttr = {instructions: instruction_num, cycle: cycle_num, miss: miss_num, access: access_num}
def loadSingle(filename):
    fp = open(filename, 'r')

    single = {}
    for line in fp.readlines():
        line = line.strip()
        if len(line) <= 0 or line[0] == '*':
            continue
        raw = line.split(' ')
        taskName = raw[0]
        instructions, cycle, miss, access = [int(x) for x in raw[1:-1]]
        single[taskName] = {
            "instructions": instructions,
            "cycle": cycle,
            "miss": miss,
            "access": access
        }

    fp.close()
    return single

# calculate CPI, IPC, Fairness
def calcMetrics(data, single):
    for scheduleObj in data:
        for taskSets in scheduleObj.values():
            # calc CPI and IPC for schedule
            cycle_schedule = instructions_schedule = 0
            slowdown = []
            # calc Fairness for schedule
            for taskSet in taskSets.values():
                for taskName in taskSet.keys():
                    taskAttr = taskSet[taskName]
                    # calc CPI and IPC for task
                    access = taskAttr['access']
                    instructions = taskAttr['instructions']
                    cycle = taskAttr['cycle']
                    miss = taskAttr['miss']
                    taskAttr['CPI'] = float(cycle) / instructions
                    taskAttr['IPC'] = float(instructions) / cycle
                    cycle_schedule += cycle
                    instructions_schedule += instructions
                    # calc slowdown for Fairness
                    slow = float(cycle) / single[taskName]['cycle']
                    slowdown.append(slow)
            # used for get pure metrics
            # keys = []
            # for key in taskSets.keys():
            #     keys.append(key)
            # for key in keys:
            #     del taskSets[key]
            taskSets['CPI'] = float(cycle_schedule) / instructions_schedule
            taskSets['IPC'] = float(instructions_schedule) / cycle_schedule
            taskSets['Unfairness'] = np.std(slowdown, ddof=1) / np.mean(slowdown)

def save(output_file, data):
    fp = open(output_file, "w")
    s = json.dumps(data, indent=4)
    fp.write(s)
    fp.close()

if __name__ == "__main__":
    SINGLE = "./corun_result/single"
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
