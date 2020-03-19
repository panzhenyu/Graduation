from src.Task import *

if __name__ == "__main__":
    t = Task("mcf")
    t.build_fetch_hit("mcf.txt")
    t.calc_sticky()
    t.show()