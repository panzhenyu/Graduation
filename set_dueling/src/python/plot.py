import matplotlib.pyplot as plt
import sys

if __name__ == "__main__":
    #申请数据
    argv = sys.argv
    if len(argv) != 2 or not argv[1].isdigit():
        print("Invalid arguments!\nPLOT_PARAM is needed!")
        sys.exit(-1)
    assoc = int(argv[1])
    test=[]
    retest=[]
    test_filename = "../result/test_" + str(assoc)
    retest_filename = "../result/retest_" + str(assoc)
    f1 = open(test_filename)
    f2 = open(retest_filename)
    test_lines = f1.readlines()
    retest_lines = f2.readlines()
    #读文件
    for i in range(len(test_lines)):
        test.append(test_lines[i].strip())
        retest.append(retest_lines[i].strip())
    test = list(map(int, map(float, test)))
    retest = list(map(int, map(float, retest)))
    #画图
    plt.title("set_dueling_" + str(assoc))
    plt.xlabel('set')
    plt.ylabel('cycle')
    plt.plot(test, 'g', label = 'test')
    plt.plot(retest, 'b', label = 'retest')
    plt.legend()
    plt.grid()
    # plt.show()
    plt.savefig("../img/set_dueling_{assoc}.png".format(assoc=assoc),format='png')
