from src.algorithm import DI4SelfAdaptive
import sys, os

def getSchedule(argc, argv, processor_num):
    di = DI4SelfAdaptive("/home/cuiran/exp_profile_parallel/co-scheduling/profile")
    for i in range(2, argc):
        di.importTask(argv[i])
    return di.solve(processor_num)

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

def loadPhaseLocMap(filename):
    mapfile = open(filename, "r")
    phaseLocMap = []
    i = 0
    for line in mapfile.readlines():
        line = line.strip()
        if len(line) != 0:
            phaseLocMap.append(line.split(" ",3))
            i += 1
    mapfile.close()
    return phaseLocMap

def test4Schedule(schedule, nameIdMap, phaseLocMap ,exec_path,result_path):
    #mapfile = open(result_path, "a+")
    for t_set in schedule.taskSets:
        idSet = taskSet2IdSet(t_set, nameIdMap)
        task_arg, task_num = "", len(idSet)
        for _id in idSet:
################################3
            phaseLocMap_id=0
            while str(_id)!=phaseLocMap[phaseLocMap_id][0]:
                phaseLocMap_id=phaseLocMap_id+1
##################################3
            #task_arg += " " + str(_id) + " 100 200"
            task_arg += " " + str(_id) + " " +phaseLocMap[phaseLocMap_id][1]+" " +phaseLocMap[phaseLocMap_id][2]
        cmd = exec_path + " " + str(task_num) + task_arg
        print("-->going to run cmd: " + cmd)
        os.system(cmd)

if __name__ == "__main__":
    # config parameters
    PROFILE_STRATEGY_PATH = "../profile_strategy"
    MAPFILE_PATH = "./benchmark_list"
    MAPPHASELOC_PATH = "./benchmark_phaes_start_end"
    RESULT_PATH="./PARALLEL_OUT"

    argc, argv = len(sys.argv), sys.argv
    if argc <= 2:
        print("usage: python3 thisfile processor_num taskName [taskName]")
        sys.exit(-1)
    processor_num = int(argv[1])

    # get schedule
    schedule = getSchedule(argc, argv, processor_num)
    print("DI4SelfAdaptive finished for {0} processor situation:".format(processor_num))
    fp = open(RESULT_PATH, "a+")
    fp.write("\n"+"DI:"+"\n"+str(schedule)+"\n")
    fp.close()
    # get name id map
    nameIdMap = loadNameIdMap(MAPFILE_PATH)

    # get start end loc !!
    phase_start_end= loadPhaseLocMap(MAPPHASELOC_PATH)
    #print("phase_start_end:" +phase_start_end[29][3])

    # simulate schedule
    test4Schedule(schedule, nameIdMap, phase_start_end ,PROFILE_STRATEGY_PATH,RESULT_PATH)

