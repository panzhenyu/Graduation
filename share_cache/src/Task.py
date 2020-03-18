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

	def build_fetch_hit(self, file_name, cache_size_idx=0, miss_num_idx=2, access_num_idx=3):
		fd = open(file_name, 'r')

		raw_text = []
		for line in fd.readlines():
			raw_text.append(line.strip().split(' '))
		raw_data = []
		for row in raw_text:
			raw_data.append([int(x) for x in row])

		cache_size = [0]
		fetch_rate = [1.0]
		hit_rate = [0.0]

		for row in raw_data:
			cache_size.append(row[cache_size_idx])
			fetch, access = row[miss_num_idx], row[access_num_idx]
			hit = access - fetch
			fetch_rate.append(float(fetch) / access)
			hit_rate.append(float(hit) / access)

		point_num = len(cache_size)
		self.hit_rate = HitRate()
		self.fetch_rate = FetchRate()

		self.hit_rate.load_data(cache_size, hit_rate)
		self.fetch_rate.load_data(cache_size, fetch_rate)

	def show(self):
		print(self.name)
		self.hit_rate.show()
		self.fetch_rate.show()
		self.sticky_cache.show()
