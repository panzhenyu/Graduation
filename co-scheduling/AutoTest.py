# auto test for task set
# task set path: co_scheduling_home/input/tast_set
# usage: python3 autoTest.py corun_task_num processor_num
# note that the corun_task_num must small than total task number in task_set file

from config.path import *
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

def getAllTask(filename):
	taskName = []
	fp = open(filename, "r")
	for line in fp.readlines():
		line = line.strip()
		if len(line) != 0:
			taskName.append(line)
	return taskName

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

def getFromDICompare(tasks, processor_num, DITaskObjs):
	scheduler = DICompare("")
	for task in tasks:
		scheduler.addTask(DITaskObjs[task])
	return scheduler.solve(processor_num)

if __name__ == "__main__":
	if len(sys.argv) != 3:
		print("usage: python3 autoTest.py corun_task_num processor_num")
		sys.exit(-1)
	corun_task_num = int(sys.argv[1])
	processor_num = int(sys.argv[2])

	taskName = getAllTask(AUTOTEST_TASKSET)
	DITaskObjs = buildDITaskObject(taskName, PROFILE_HOME)
	taskSets = sub(taskName, corun_task_num)

	result = open(AUTOTEST_COMBINATION, "w")
	needed = open(AUTOTEST_DIFFFILE, "w")
	diff = set([])

	seq = 0
	diff_num = 0
	for tasks in taskSets:
		# di_result = getFromDI(tasks, processor_num, DITaskObjs)
		diImprove_result = getFromDI4SelfAdaptive(tasks, processor_num, DITaskObjs)
		diCompare_result = getFromDICompare(tasks, processor_num, DITaskObjs)
		# head line, used to differ schedule result order
		result.write("diImprove diCompare")
		if diImprove_result != diCompare_result:
			result.write("seq:" + str(seq) + "\n")
			result.write(str(diImprove_result) + "\n")
			result.write("---\n")
			result.write(str(diCompare_result) + "\n\n")
			seq += 1
		for t_set in diImprove_result.getTaskSets():
			if t_set.length() != 1:
				diff.add(str(t_set))
		for t_set in diCompare_result.getTaskSets():
			if t_set.length() != 1:
				diff.add(str(t_set))

	seq = 0
	for t_set_str in diff:
		needed.write(str(seq) + ":" + t_set_str + "\n")
		seq += 1

	needed.close()
	result.close()
