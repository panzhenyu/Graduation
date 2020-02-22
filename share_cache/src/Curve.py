from .Error import Error

class Curve:

	def __init__(self, x_axis, y_axis):
		self.x = []
		self.y = []
		self.x_axis = None
		self.y_axis = None
		self.point_num = 0

	def load(self, filename, format):
		if format == 'csv':
			pass

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
			return self.y[i-1] + self.y[i] * (x - self.x[i-1]) / (self.x[i] - self.x[i-1])

	def getX(self, y):
		pass

# class MissRate(Curve):

# 	def __init__(self):
# 		Curve.__init__(self, "cache size", "miss rate")

class HitRate(Curve):
	
	def __init__(self):
		Curve.__init__(self, "cache size", "hit rate")

class FetchRate(Curve):

	def __init__(self):
		Curve.__init__(self, "fetch rate", "hit rate")

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
		return int(super(StickyCache, self).getY(x))

	# this way, when the neighbour points is close enough, we use one of their x directly to decrease the error
	def getX(self, y):
		for i in range(1, self.point_num):
			if y >= self.y[i-1] and y <= self.y[i]:
				if abs(self.y[i-1] - self.y[i]) <= 1e-3:
					return self.y[i]
				return self.x[i] - (self.y[i] - y) / (self.y[i] - self.y[i-1]) * (self.x[i] - self.x[i-1])
			elif y <= self.y[i-1] and y >=self.y[i]:
				if abs(self.y[i-1] - self.y[i]) <= 1e-3:
					return self.y[i]
				return (self.y[i-1] - y) / (self.y[i-1] - self.y[i]) * (self.x[i] - self.x[i-1]) + self.x[i-1]
		return None
