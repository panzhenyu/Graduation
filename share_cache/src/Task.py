from .Curve import *

# runtime profile of a task
class Task:

	def __init__(self, name):
		self.name = str(name)
		# self.miss_rate = None
		self.hit_rate = None
		self.fetch_rate = None
		self.sticky_cache = None

	def load_miss(self, filename, format=None):
		self.miss_rate = MissRate()
		self.miss_rate.load(filename, format)

	def load_hit(self, filename, format=None):
		self.hit_rate = HitRate()
		self.hit_rate.load(filename, format)

	def load_fetch(self, filename, format=None):
		self.fetch_rate = FetchRate()
		self.fetch_rate.load(filename, format)

	def calc_sticky(self):
		self.sticky_cache = StickyCache()
		self.sticky_cache.load(self.hit_rate)
