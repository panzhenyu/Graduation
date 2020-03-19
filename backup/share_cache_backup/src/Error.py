import sys

class Error:
	def perror(msg):
		print("ERROR:", msg)
		sys.exit(-1)