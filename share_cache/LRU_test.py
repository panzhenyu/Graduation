from src.Models import LRU
from src.Curve import *
from src.Task import Task

if __name__ == "__main__":
	task1, task2 = Task('task1'), Task('task2')
	hit_rate1, hit_rate2 = HitRate(), HitRate()
	fetch_rate1, fetch_rate2 = FetchRate(), FetchRate()

	hit_rate1.x = [x for x in range(9)]
	hit_rate2.x = [x for x in range(9)]
	fetch_rate1.x = [x for x in range(9)]
	fetch_rate2.x = [x for x in range(9)]

	hit_rate1.y = [0, 0.21, 0.34, 0.36, 0.48, 0.49, 0.49, 0.55, 0.9]
	fetch_rate1.y = [1, 0.6, 0.55, 0.5, 0.47, 0.45, 0.44, 0.43, 0.2]
	hit_rate2.y = [0, 0.01, 0.02, 0.21, 0.23, 0.25, 0.25, 0.29, 0.3]
	fetch_rate2.y = [1, 0.98, 0.96, 0.74, 0.72, 0.7, 0.7, 0.7, 0.7]

	hit_rate1.point_num = hit_rate2.point_num = fetch_rate1.point_num = fetch_rate2.point_num = 9

	task1.hit_rate, task2.hit_rate = hit_rate1, hit_rate2
	task1.fetch_rate, task2.fetch_rate = fetch_rate1, fetch_rate2
	task1.calc_sticky()
	task2.calc_sticky()

	print(task1.sticky_cache.y)
	print(task2.sticky_cache.y)

	tasks, lru = [task1, task2], LRU(size=8, association=64, sets=2048, cache_line_size=64, shrink=0.1)
	lru.solve(tasks)
	tasks = [task2, task1]
	lru.solve(tasks)
