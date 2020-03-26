import os

def taskSet2IdSet(taskSet, nameIdMap):
    idSet = []
    names = [x.name for x in taskSet.getTaskList()]
    for name in names:
        idSet.append(nameIdMap[name])
    return idSet

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

def test4Schedule(schedule, nameIdMap, exec_path):
    for t_set in schedule.taskSets:
        idSet = taskSet2IdSet(t_set, nameIdMap)
        task_arg, task_num = "", len(idSet)
        for _id in idSet:
            task_arg += " " + str(_id) + " 100 200"
        cmd = exec_path + " " + str(task_num) + task_arg
        print("-->going to run cmd: " + cmd)
        os.system(cmd)
