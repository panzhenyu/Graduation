from src.algorithm import DI
from config.path import *
from utils import *
import sys

def getSchedule(argc, argv, processor_num, profile_home):
    di = DI(profile_home)
    for i in range(2, argc):
        di.importTask(argv[i])
    return di.solve(processor_num)

if __name__ == "__main__":
    argc, argv = len(sys.argv), sys.argv
    if argc <= 2:
        print("usage: python3 thisfile processor_num taskName [taskName]")
        sys.exit(-1)
    processor_num = int(argv[1])

    # get schedule
    schedule = getSchedule(argc, argv, processor_num, PROFILE_HOME)
    print("DI4SelfAdaptive finished for {0} processor situation:".format(processor_num))
    print(str(schedule))

    # get maps
    nameIdMap = loadNameIdMap(TASKID_PATH)
    idPhaseMap = loadIdPhaseMap(PHASE_PATH)

    # simulate schedule
    runSchedule(schedule, nameIdMap, idPhaseMap, PROFILE_STRATEGY)
