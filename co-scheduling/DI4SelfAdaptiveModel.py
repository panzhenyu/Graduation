from src.algorithm import DI4SelfAdaptive
import sys

if __name__ == "__main__":
    argc = len(sys.argv)
    if argc <= 2:
        print("usage: python3 $thisfile $processor_num $tasks")
        sys.exit(-1)
    di, argv = DI4SelfAdaptive(), sys.argv
    for i in range(2, argc):
        di.importTask(argv[i])
    di.solve(int(argv[1]))

