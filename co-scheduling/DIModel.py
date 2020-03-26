from src.algorithm import DI
from corun_utils import *
import sys

def getSchedule(argc, argv, processor_num, profile_home):
    di = DI(profile_home)
    for i in range(2, argc):
        di.importTask(argv[i])
    return di.solve(processor_num)

if __name__ == "__main__":
    # config parameters
    PROFILE_STRATEGY_PATH = "../profile_strategy"
    MAPFILE_PATH = "./benchmark_list"
    PROFILE_HOME = "/home/panda/Desktop/Graduation/co-scheduling/profile"

    argc, argv = len(sys.argv), sys.argv
    if argc <= 2:
        print("usage: python3 thisfile processor_num taskName [taskName]")
        sys.exit(-1)
    processor_num = int(argv[1])

    # get schedule
    schedule = getSchedule(argc, argv, processor_num, PROFILE_HOME)
    print("DI4SelfAdaptive finished for {0} processor situation:".format(processor_num))
    print(str(schedule))

    # get name id map
    nameIdMap = loadNameIdMap(MAPFILE_PATH)

    # simulate schedule
    test4Schedule(schedule, nameIdMap, PROFILE_STRATEGY_PATH)
