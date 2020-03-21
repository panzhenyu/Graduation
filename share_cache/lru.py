from src.Models import *
from src.Task import *
import sys

if __name__ == "__main__":
	argc = len(sys.argv)
	if argc <= 2:
		print("num of arguments must larger than 2!")
		sys.exit(1)
	task_name, tasks = [], []
	for i in range(1, argc):
		task_name.append(sys.argv[i])
	for name in task_name:
		tasks.append(Task(name))
	for task in tasks:
		task.build_fetch_hit(task.name + ".data")
		task.calc_sticky()
		task.show()

	lru = LRU(size=8388608, association=64, sets=2048, cache_line_size=64, shrink=512)
	lru.solve(tasks)
