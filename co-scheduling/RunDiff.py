from AutoTest import *
from config.home import *
from utils import *

def loadDiff(diff_file):
	result = []
	diff = open(diff_file, "r")
	for line in diff.readlines():
		line = line.strip()
		if len(line) > 0:
			task_set = line.split(":")[1].split(" ")
			result.append(task_set)
	diff.close()
	return result

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
	MAPFILE_PATH = CO_SCHEDULING_HOME + "/config/txt/benchmark_list"
	PHASE_PATH = CO_SCHEDULING_HOME + "/config/txt/benchmark_phase"
	DIFF_FILE = CO_SCHEDULING_HOME + "/output/corun_set_needed"
	PROFILE_STRATEGY_PATH = "../profile_strategy"

	taskSets = loadDiff(DIFF_FILE)
	nameIdMap = loadNameIdMap(MAPFILE_PATH)
	idPhaseMap = loadIdPhaseMap(PHASE_PATH)
	set_num = len(taskSets)

	runTaskSets(0, set_num, taskSets, nameIdMap, idPhaseMap, PROFILE_STRATEGY_PATH)
