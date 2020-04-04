from itertools import combinations
from src.algorithm import DI
import os

def loadNameIdMap(filename):
    mapfile = open(filename, "r")
    nameIdMap = {}
    i = 0
    for line in mapfile.readlines():
        line = line.strip()
        if len(line) != 0:
            nameIdMap[line] = i
            i += 1
    mapfile.close()
    return nameIdMap

def loadIdPhaseMap(filename):
    mapfile = open(filename, "r")
    idPhaseMap = {}
    for line in mapfile.readlines():
        line = line.strip()
        if len(line) != 0:
            raw_data = line.split(" ")[0: 3]
            _id, s, e = [int(x) for x in raw_data]
            idPhaseMap[_id] = [s, e]
    mapfile.close()
    return idPhaseMap

def taskNameSet2IdSet(taskNameSet, nameIdMap):
    idSet = []
    for name in taskNameSet:
        idSet.append(nameIdMap[name])
    return idSet

def taskSet2NameSet(taskSet):
    nameSet = [x.name for x in taskSet.getTaskList()]
    return nameSet

def taskCorun(taskNameSet, nameIdMap, idPhaseMap, exec_path):
    idSet = taskNameSet2IdSet(taskNameSet, nameIdMap)
    task_arg, task_num = "", len(taskNameSet)
    for _id in idSet:
        phase_start, phase_end = idPhaseMap[_id]
        task_arg += " " + str(_id) + " " + str(phase_start) + " " + str(phase_end)
    cmd = exec_path + " " + str(task_num) + task_arg
    print("-->execute:", cmd)
    os.system(cmd)

def runSchedule(schedule, nameIdMap, idPhaseMap, exec_path):
    for t_set in schedule.taskSets:
        nameSet = taskSet2NameSet(t_set)
        taskCorun(nameSet, nameIdMap, idPhaseMap, exec_path)

def sub(_list, _n):
    result = []
    for s in combinations(_list, _n):
        result.append(list(s))
    return result

# result is a list:
# result = [taskSet, ...]
# taskSet = [taskName, ...]
def loadDiff(diff_file):
    result = []
    diff = open(diff_file, "r")
    for line in diff.readlines():
        line = line.strip()
        if len(line) > 0:
            task_set = line.split(" ")
            result.append(task_set)
    diff.close()
    return result

# taskSets = [taskSet, ...]
# taskSet = [taskName, ...]
# taskName = str
def saveDiff(taskSets, filename):
    diff = open(filename, "w")
    for t_set in taskSets:
        t_set.sort()
        t_set_str = ""
        for taskName in t_set:
            t_set_str += taskName + " "
        t_set_str = t_set_str[:-1] + '\n'
        diff.write(t_set_str)
    diff.close()

# note that the result file separator is '*'， so the first letter of taskName cannot be '*'
# the taskNameSeq must consist of sorted taskNames
# corun result is list format:
# corun_result = {taskNameSeq: taskSet, ...}
# taskSet = {taskName: taskAttr, ...}
# taskAttr = {instructions: instruction_num, cycle: cycle_num, miss: miss_num, access: access_num}
def loadCorunResult(filename):
    fp = open(filename, "r")

    corun_result, taskNames, collector = {}, [], {}
    for line in fp.readlines():
        line = line.strip()
        if len(line) <= 0:
            continue
        if line[0] == '*':
            if len(collector) != 0:
                taskNames.sort()
                taskNameSeq = ""
                for name in taskNames:
                    taskNameSeq += name + " "
                taskNameSeq = taskNameSeq[0:-1]
                corun_result[taskNameSeq] = collector
            taskNames, collector = [], {}
        else:
            taskAttr = {}
            data = line.split(' ')
            taskName = data.pop(0)
            attrName = ["instructions", "cycle", "miss", "access"]
            for i in range(len(attrName)):
                taskAttr[attrName[i]] = int(data[i])
            collector[taskName] = taskAttr
            taskNames.append(taskName)
    if len(collector) != 0:
        taskNames.sort()
        taskNameSeq = ""
        for name in taskNames:
            taskNameSeq += name + " "
        taskNameSeq = taskNameSeq[0:-1]
        corun_result[taskNameSeq] = collector

    fp.close()
    return corun_result

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
        instructions, cycle, miss, access = [int(x) for x in raw[1:5]]
        single[taskName] = {
            "instructions": instructions,
            "cycle": cycle,
            "miss": miss,
            "access": access
        }

    fp.close()
    return single

def buildDITaskObject(tasks, profile_home):
    result = {}
    di = DI(profile_home)
    for task in tasks:
        di.importTask(task)
    for task in di.getTaskSet().getTaskList():
        result[task.name] = task
    return result
