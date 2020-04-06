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

def getAllTask(filename):
	taskName = []
	fp = open(filename, "r")
	for line in fp.readlines():
		line = line.strip()
		if len(line) != 0:
			taskName.append(line)
	return taskName

def getDISeriesResult(scheduleAlgorithm, tasks, processor_num, DITaskObjs):
	diSeries = scheduleAlgorithm("")
	for task in tasks:
		diSeries.addTask(DITaskObjs[task])
	return diSeries.solve(processor_num)

if __name__ == "__main__":
	if len(sys.argv) != 3:
		print("usage: python3 autoTest.py corun_task_num processor_num")
		sys.exit(-1)
	corun_task_num = int(sys.argv[1])
	processor_num = int(sys.argv[2])

	taskName = getAllTask(AUTOTEST_TASKSET)
	DITaskObjs = buildDITaskObject(taskName, PROFILE_HOME)
	taskSets = sub(taskName, corun_task_num)

	combinations = open(AUTOTEST_COMBINATION, "w")
	needed = open(AUTOTEST_CURDIFF, "w")

	seq = 0
	diff = set([])
	diSeriesAlgorithm = [DI4CompareImproveSelect, DI]

	# save head line, used to differ schedule result order
	headLine = ""
	for diSeries in diSeriesAlgorithm:
		headLine += diSeries.name + " "
	headLine = headLine[:-1] + '\n'
	combinations.write(headLine)
	# collect result for every taskSet
	for tasks in taskSets:
		# collect schedule result
		results = []
		for diSeries in diSeriesAlgorithm:
			schedule = getDISeriesResult(diSeries, tasks, processor_num, DITaskObjs)
			results.append(schedule)
		# make sure that there are difference in results
		diff_result = False
		for i in range(1, len(results)):
			if results[i] != results[0]:
				diff_result = True
				break
		# if the difference exists, save all schedule results
		if diff_result:
			combinations.write("seq:" + str(seq) + "\n")
			for result in results:
				# save combination result file
				combinations.write(str(result) + "\n")
				combinations.write("---\n")
				# add task set into diff set
				for t_set in result.getTaskSets():
					if t_set.length() != 1:
						diff.add(str(t_set))
			combinations.write("\n")
			seq += 1

	for t_set_str in diff:
		needed.write(t_set_str + "\n")

	print("total:{0}, diff:{1}".format(len(taskSets), seq))
	needed.close()
	combinations.close()
