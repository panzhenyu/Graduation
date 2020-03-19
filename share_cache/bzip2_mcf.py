from src.Models import *
from src.Task import *

if __name__ == "__main__":
	bzip2 = Task("bzip")
	mcf = Task("mcf")

	bzip2.build_fetch_hit("bzip2.data")
	mcf.build_fetch_hit("mcf.data")

	bzip2.calc_sticky()
	mcf.calc_sticky()

	
	lru = LRU(size=8388608, association=64, sets=2048, cache_line_size=64, shrink=512)
	lru.solve([bzip2, mcf])
