# auto test for task set
# task set path: co_scheduling_home/input/tast_set
# usage: python3 autoTest.py corun_task_num processor_num
# note that the corun_task_num must small than total task number in task_set file

from config.home import *
from src.task import *
from src.schedule import *
from src.algorithm import *
from utils import *
import sys

def buildDITaskObject(tasks, profile_home):
	result = {}
	di = DI(profile_home)
	for task in tasks:
		di.importTask(task)
	for task in di.getTaskSet().getTaskList():
		result[task.name] = task
	return result

def getFromDI(tasks, processor_num, DITaskObjs):
	scheduler = DI("")
	for task in tasks:
		scheduler.addTask(DITaskObjs[task])
	return scheduler.solve(processor_num)

def getFromDI4SelfAdaptive(tasks, processor_num, DITaskObjs):
	scheduler = DI4SelfAdaptive("")
	for task in tasks:
		scheduler.addTask(DITaskObjs[task])
	return scheduler.solve(processor_num)

def getAllTask(filename):
	taskName = []
	fp = open(filename, "r")
	for line in fp.readlines():
		line = line.strip()
		if len(line) != 0:
			taskName.append(line)
	return taskName

if __name__ == "__main__":
	taskSetPath = CO_SCHEDULING_HOME + "/input/task_set"
	profile_home = CO_SCHEDULING_HOME + "/profile"
	output = CO_SCHEDULING_HOME + "/output/task_set_result"
	needcorun = CO_SCHEDULING_HOME + "/output/corun_set_needed"

	if len(sys.argv) != 3:
		print("usage: python3 autoTest.py corun_task_num processor_num")
		sys.exit(-1)
	corun_task_num = int(sys.argv[1])
	processor_num = int(sys.argv[2])

	taskName = getAllTask(taskSetPath)
	DITaskObjs = buildDITaskObject(taskName, profile_home)
	taskSets = sub(taskName, corun_task_num)

	result = open(output, "w")
	needed = open(needcorun, "w")
	diff = set([])

	seq = 0
	for tasks in taskSets:
		di_result = getFromDI(tasks, processor_num, DITaskObjs)
		diImprove_result = getFromDI4SelfAdaptive(tasks, processor_num, DITaskObjs)
		if di_result != diImprove_result:
			result.write("seq:" + str(seq) + "\n")
			result.write(str(di_result) + "\n")
			result.write("---\n")
			result.write(str(diImprove_result) + "\n\n")
			seq += 1
		for t_set in di_result.getTaskSets():
			diff.add(str(t_set))
		for t_set in diImprove_result.getTaskSets():
			diff.add(str(t_set))

	seq = 0
	for t_set_str in diff:
		needed.write(str(seq) + ":" + t_set_str + "\n")
		seq += 1

	needed.close()
	result.close()


