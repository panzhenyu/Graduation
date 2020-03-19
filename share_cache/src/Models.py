import platform
from .Error import *
from .Task import Task

class Cache:

	def __init__(self, size=None, association=None, sets=None, cache_line_size=None, latency=None):
		if size == None and association==None and sets==None and cache_line_size==None and latency==None:
			self.load_platform()
		else:
			self.size = size
			self.association = association
			self.sets = sets
			self.cache_line_size = cache_line_size
			self.latency = latency

	def load_platform(self):
		self.size = None
		self.association = None
		self.sets = None
		self.sets = None
		self.cache_line_size = None
		self.latency = None

		bit, os = list(map(str.lower, platform.architecture()))
		if 'windows' in os:
			pass
		elif 'linux' in os:
			pass
		else:
			print("Unrecognized Operating System!")

class Random(Cache):

	def __init__(self, size=None, association=None, sets=None, cache_line_size=None, latency=None):
		# notice that the dimension of size should fit curves
		Cache.__init__(self, size, association, sets, cache_line_size, latency)

	def solve(self, tasks):
		pass

class LRU(Cache):
	
	def __init__(self, size=None, association=None, sets=None, cache_line_size=None, latency=None, shrink=0.5):
		# notice that the dimension of size should fit curves
		Cache.__init__(self, size, association, sets, cache_line_size, latency)
		self.shrink = shrink
		# runtime attribute
		self.tasks = None
		self.allocation = {}
		self.volatile = 0

	def __clean(self):
		self.task = None
		self.allocation = {}
		self.volatile = 0

	def __load_tasks(self, tasks):
		self.tasks = tasks

	def sticky_age(self, task):
		# function to evaluate age
		sticky = self.allocation[task.name]
		owned = task.sticky_cache.getX(sticky)
		if owned == 0:
			return 0
		hit = task.hit_rate.getY(owned)
		return sticky / hit
	
	def max_volatile_age(self):
		F = 0
		alloc = self.allocation
		for task in self.tasks:
			sticky = alloc[task.name]
			owned = task.sticky_cache.getX(sticky)
			F += task.fetch_rate.getY(owned)
		# 1e-3 is an experienced value
		if F <= 1e-3:
			return float('inf')
		return self.volatile / F

	def isLegal(self, task):
		# check for the task's constraint
		age_sticky = self.sticky_age(task)
		age_volatile = self.max_volatile_age()
		print("!!!age_sticky: {sticky}  age_volatile: {volatile}".format(sticky=age_sticky, volatile=age_volatile));
		return age_sticky <= age_volatile

	def adjust_allocation(self, task):
		# adjust the task's cache allocation
		rate = self.shrink
		alloc = self.allocation
		while not self.isLegal(task):
			owned = alloc[task.name]
			if owned == 0:
				break
			decrease = owned if rate > owned else rate
			alloc[task.name] -= decrease
			self.volatile += decrease

	def init_allocation(self):
		# init cache allocation for each task
		rest = self.size
		alloc = self.allocation
		for task in self.tasks:
			alloc[task.name] = sticky = task.sticky_cache.getY(rest)
			if sticky > rest:
				Error.perror("The sticky part cannot larger than input cache size, error occurs within sticky cache curve!\n"
							"Input cache size : " + str(rest) + "\n"
							"Sticky cache : " + str(sticky) + "\n"
							"Task name : " + task.name + "\n")
			rest -= sticky
		self.volatile = rest

	def allocate_volatile(self):
		fetch = {}
		alloc = self.allocation
		for task in self.tasks:
			sticky = alloc[task.name]
			owned = task.sticky_cache.getX(sticky)
			fetch[task.name] = task.fetch_rate.getY(owned)
		sum_fetch = sum(fetch.values())

		if self.volatile == 0 or sum_fetch == 0:
			return

		rest = self.volatile
		for task_name in fetch.keys():
			volatile_cache = int(fetch[task_name] / sum_fetch * self.volatile)
			alloc[task_name] += volatile_cache
			rest -= volatile_cache
		alloc[task_name] += rest

	def solve(self, tasks):
		print("---solve start---")
		self.__clean()
		self.__load_tasks(tasks)

		self.init_allocation()
		print("init_allocation done");
		change = True
		while(change):
			print(self.allocation, self.volatile)
			change = False
			max_age, max_task = -1, None
			for task in self.tasks:
				if not self.isLegal(task):
					a_s = self.sticky_age(task)
					if a_s > max_age:
						max_age, max_task = a_s, task
			if max_task != None:
				change = True
				self.adjust_allocation(max_task)
		self.allocate_volatile()
		self.result()
		print("---solve end---")

	def result(self):
		print("LRU Cache shared Model finished:")
		alloc = self.allocation
		for task in self.tasks:
			size = alloc[task.name]
			print("\t{name} : alloc:{allocation} hit_rate:{hit_rate} fetch_rate:{fetch_rate} ".format(
					name=task.name,
					allocation=size,
					hit_rate=task.hit_rate.getY(size),
					fetch_rate=task.fetch_rate.getY(size)
			))
