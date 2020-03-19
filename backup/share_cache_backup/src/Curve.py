from .Error import Error

class Curve:

	def __init__(self, x_axis, y_axis):
		self.x = []
		self.y = []
		self.x_axis = str(x_axis)
		self.y_axis = str(y_axis)
		self.point_num = 0

	def load_file(self, filename, format):
		if format == 'csv':
			pass

	def load_data(self, x, y):
		point_num = len(x)
		if point_num != len(y):
			Error.perror("len(x) != len(y), something error while build load_data!\n")
		self.x, self.y = x[:], y[:]
		self.point_num = point_num

	def plot(self):
		pass

	# the point is discrete, so GET methods use linear model to give a approximate value
	def getY(self, x):
		# make sure the x list is in order
		if self.point_num <= 0:
			Error.perror("point_num illegal : " + str(self.point_num))
		elif x < self.x[0] or x > self.x[-1]:
			Error.perror("x:" + str(x) + " is out of range!\n"
							"\tExpected x : [" + str(self.x[0]) + ", " + str(self.x[-1]) + "]")
		for i in range(self.point_num):
			if x <= self.x[i]:
				break
		if x == self.x[i]:
			return self.y[i]
		elif i-1 >= 0:
			bias = (x - self.x[i-1]) / (self.x[i] - self.x[i-1]) * abs(self.y[i] - self.y[i-1])
			if self.y[i] > self.y[i-1]:
				return self.y[i] - bias
			else:
				return self.y[i-1] - bias

	def getX(self, y):
		pass
	
	def show(self):
		print("point_num: ", self.point_num, "\n", self.x_axis, self.x, "\n", self.y_axis, self.y, "\n")

# class MissRate(Curve):

# 	def __init__(self):
# 		Curve.__init__(self, "cache size", "miss rate")

class HitRate(Curve):
	
	def __init__(self):
		Curve.__init__(self, "cache size", "hit rate")

class FetchRate(Curve):

	def __init__(self):
		Curve.__init__(self, "cache size", "fetch rate")

class StickyCache(Curve):
	
	def __init__(self):
		Curve.__init__(self, "cache size", "sticky cache size")

	def load(self, hitrate):
		self.point_num = hitrate.point_num
		self.x = hitrate.x.copy()
		self.y = [0] * self.point_num
		for i in range(1, self.point_num):
			h = hitrate.getY(hitrate.x[i])
			preh = hitrate.getY(hitrate.x[i-1])
			self.y[i] = self.y[i-1] + (h - preh) * (self.x[i] - self.y[i-1]) / (1 - preh) if preh != 1 else self.y[i-1]

	def getY(self, x):
		return super(StickyCache, self).getY(x)

	# this way, when the neighbour points is close enough, we use one of their x directly to decrease the error
	def getX(self, y):
		for i in range(1, self.point_num):
			if self.y[i-1] > self.y[i]:
				max_idx, min_idx = i-1, i
			else:
				max_idx, min_idx = i, i-1
			max_x, min_x = self.x[max_idx], self.x[min_idx]
			max_y, min_y = self.y[max_idx], self.y[min_idx]

			if y <= max_y and y >= min_y:
				bias = (y - min_y) / (max_y - min_y) * (max_x - min_x)
				if max_idx == i:
					return bias + min_x
				else:
					return max_x - bias
		return None
