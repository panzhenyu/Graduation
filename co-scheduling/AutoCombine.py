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
    if len(sys.argv) != 2:
        print("usage: python3 Combination.py combNum")
        sys.exit(-1)
    combNum = int(sys.argv[1])
    taskName = getAllTask(AUTOTEST_TASKSET)
    combinations = sub(taskName, combNum)
    saveDiff(list(combinations), AUTOTEST_CURDIFF)
