import matplotlib.pyplot as plt
import sys

if __name__ == "__main__":
	#申请数据
	argv = sys.argv
	if len(argv) != 2 or not argv[1].isdigit():
		print("Invalid arguments!")
		sys.exit(-1)
	assoc = int(argv[1])
	test=[]
	retest=[]
	test_filename = "./result/test.txt"
	retest_filename = "./result/retest.txt"
	f1=open(test_filename)
	f2=open(retest_filename)
	n_line = 4096
	#读文件
	for i in range(n_line):
		test.append(f1.readline().strip())
		retest.append(f2.readline().strip())
	test = list(map(int, map(float, test)))
	retest = list(map(int, map(float, retest)))
	#画图
	plt.title("set_dueling")
	plt.xlabel('set')
	plt.ylabel('cycle')
	plt.plot(test, 'g', label = 'test')
	plt.plot(retest, 'b', label = 'retest')
	plt.legend()
	plt.grid()
	# plt.show()
	plt.savefig("./img/set_dueling_{assoc}".format(assoc=assoc),format='png')
