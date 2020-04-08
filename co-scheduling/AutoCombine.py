from utils import *
from config.path import *
import sys

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: python3 Combination.py combNum")
        sys.exit(-1)
    combNum = int(sys.argv[1])
    taskName = getAllTask(AUTOTEST_TASKSET)
    combinations = sub(taskName, combNum)
    saveDiff(list(combinations), AUTOTEST_CURDIFF)
