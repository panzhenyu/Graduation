from utils import *
from config.path import *
import sys

def getAllTask(filename):
	taskName = []
	fp = open(filename, "r")
	for line in fp.readlines():
		line = line.strip()
		if len(line) != 0:
			taskName.append(line)
	return taskName

if __name__ == "__main__":
    if len(sys.argv) !=2 :
        print("usage: python3 Combination.py combNum")
        sys.exit(-1)
    combNum = int(sys.argv[1])

    taskName = getAllTask(AUTOTEST_TASKSET)
    nameIdMap = loadNameIdMap(TASKID_PATH)
    idPhaseMap = loadIdPhaseMap(PHASE_PATH)
    combinations = sub(taskName, combNum)

    for c in combinations:
        taskCorun(c, nameIdMap, idPhaseMap, PROFILE_STRATEGY)
