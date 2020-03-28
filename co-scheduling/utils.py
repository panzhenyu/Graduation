from itertools import combinations
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
