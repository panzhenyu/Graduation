from config.path import *
from utils import *

# run [begin, end)
def runTaskSets(begin, end, taskSets, nameIdMap, idPhaseMap, exec_path):
	set_num = len(taskSets)
	if begin < 0 or end > set_num:
		print("begin: {0} end: {1} is invalid".format(begin, end))
		return
	for i in range(begin, end):
		t_set = taskSets[i]
		taskCorun(t_set, nameIdMap, idPhaseMap, exec_path)

if __name__ == "__main__":
	curTaskSets = loadDiff(AUTOTEST_CURDIFF)
	reservedTaskSets = loadDiff(AUTOTEST_ALLDIFF)
	nameIdMap = loadNameIdMap(TASKID_PATH)
	idPhaseMap = loadIdPhaseMap(PHASE_PATH)

	taskSets = []
	for t_set in curTaskSets:
		if t_set not in reservedTaskSets:
			taskSets.append(t_set)
	set_num = len(taskSets)

	saveDiff(taskSets, AUTOTEST_CUR_ALL_DIFF)
	runTaskSets(0, set_num, taskSets, nameIdMap, idPhaseMap, PROFILE_STRATEGY)
