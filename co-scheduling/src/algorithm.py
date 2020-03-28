from .task import *
from .profile import *
from .schedule import *
import sys
import numpy as np

class CoScheduleAlgorithm:
    def __init__(self):
        self.taskSet = TaskSet()

    def importTask(self, taskName):
        pass

    def addTask(self, task):
        pass
    
    def solve(self):
        pass
    
    def clean(self):
        del self.taskSet
        self.taskSet = TaskSet()

    def getTaskSet(self):
        return self.taskSet

class DI4SelfAdaptive(CoScheduleAlgorithm):
    def __init__(self, profile_home):
        CoScheduleAlgorithm.__init__(self)
	# this attribute is only useful for importTask method
        self.PROFILE_HOME = str(profile_home)

    # DI profile format: one line with miss under A strategy and miss under B strategy
    # two misses are divided by space
    # DI profile path: PROFILE_HOME/${taskName}/DI
    def importTask(self, taskName):
        profile_fd = open(self.PROFILE_HOME + "/" + str(taskName) + "/DI", "r")
        miss = profile_fd.readline().strip().split(' ')
        if len(miss) < 2:
            print("in DI4SelfAdaptive importProfile, the {taskName} profile format is wrong!"
                    .format(taskName=taskName))
            sys.exit(-1)
        # modify
        miss_A, miss_B = [float(m) for m in miss]
        miss_Adaptive = 0
        task, profile = Task(taskName), DIProfile(miss_A, miss_B, miss_Adaptive)
        task.loadProfile(profile)
        self.taskSet.addTask(task)

        profile_fd.close()

    def addTask(self, task):
        self.taskSet.addTask(task)

    def taskMiss(self, task):
        profile = task.profile
        if not isinstance(profile, DIProfile):
            print("in DI4SelfAdaptive.taskMiss, the profile isn't a instance of DIProfile")
            sys.exit(-1)
        m_A, m_B = profile.miss_A, profile.miss_B
        return min(m_A, m_B)

    # from big to small
    def taskSort(self, tasks):
        miss = []
        _tasks = []
        task_num = len(tasks)

        for task in tasks:
            miss.append(self.taskMiss(task))

        idx = sorted(range(task_num), key=lambda k: miss[k])
        idx.reverse()
        for i in idx:
            _tasks.append(tasks[i])

        return _tasks

    # return a standard deviation list for every possible situation
    def stdDeviation(self, schedule, task):
        std = []
        miss_sum, task_miss = [], self.taskMiss(task)
        for t_set in schedule.taskSets:
            s_sum = 0
            for task in t_set.tasks:
                s_sum += self.taskMiss(task)
            miss_sum.append(s_sum)

        for i in range(len(miss_sum)):
            miss_sum[i] += task_miss
            std.append(np.std(miss_sum, ddof=1))
            miss_sum[i] -= task_miss
        return std

    def chooseTaskSet(self, schedule, task):
        std = self.stdDeviation(schedule, task)
        min_idx, min_var = 0, std[0]
        for i in range(1, len(std)):
            if std[i] < min_var:
                min_idx, min_var = i, std[i]
        schedule.taskSets[min_idx].addTask(task)

    def solve(self, processor_num):
        taskSetLen = self.taskSet.length()
        if taskSetLen == 0 or processor_num <= 0:
            print("in DI4SelfAdaptive.solve, the processor_num {0} or taskSet length {1} is invalid"
                    .format(processor_num, taskSetLen))
            return None
        if processor_num > taskSetLen:
            print("in DI4SelfAdaptive.solve, the processor_num {0} smaller than taskSet length {1}".
                    format(processor_num, taskSetLen))
            return None

        tasks = self.taskSort(self.taskSet.getTaskList())

        schedule = Schedule()
        for i in range(processor_num):
            t_set = TaskSet()
            t_set.addTask(tasks.pop(0))
            schedule.addTaskSet(t_set)

        if processor_num == 1:
            for task in tasks:
                schedule.taskSets[0].addTask(task)
        else:
            for task in tasks:
                self.chooseTaskSet(schedule, task)
        return schedule

class DI(CoScheduleAlgorithm):
    def __init__(self, profile_home):
        CoScheduleAlgorithm.__init__(self)
        self.PROFILE_HOME = profile_home
        self.DI4SelfAdaptive = DI4SelfAdaptive(self.PROFILE_HOME)
    
    def clean(self):
        self.DI4SelfAdaptive.clean()
        CoScheduleAlgorithm.clean(self)

    def importTask(self, taskName):
        self.DI4SelfAdaptive.importTask(taskName)

    def addTask(self, task):
        self.DI4SelfAdaptive.addTask(task)

    def getTaskSet(self):
        return self.DI4SelfAdaptive.getTaskSet()

    def solve(self, processor_num):
        orig_tasks = self.DI4SelfAdaptive.taskSet.getTaskList()
        tasks = self.DI4SelfAdaptive.taskSort(orig_tasks)
        schedule = Schedule()

        for i in range(processor_num):
            schedule.addTaskSet(TaskSet())
    
        i, d = 0, 1
        for task in tasks:
            schedule.taskSets[i].addTask(task)
            i += d
            if i == processor_num:
                d = -1
                i -= 1
            elif i == -1:
                d = 1
                i += 1
        return schedule

class DI4StrategyA(DI4SelfAdaptive):
    def __init__(self, profile_home):
        DI4SelfAdaptive.__init__(self, profile_home)

    def taskMiss(self, task):
        profile = task.profile
        if not isinstance(profile, DIProfile):
            print("in DI4StrategyA.taskMiss, the profile isn't a instance of DIProfile")
            sys.exit(-1)
        return profile.miss_A

class DI4StrategyB(DI4SelfAdaptive):
    def __init__(self, profile_home):
        DI4SelfAdaptive.__init__(self, profile_home)

    def taskMiss(self, task):
        profile = task.profile
        if not isinstance(profile, DIProfile):
            print("in DI4StrategyB.taskMiss, the profile isn't a instance of DIProfile")
            sys.exit(-1)
        return profile.miss_B

class DICompare(DI4SelfAdaptive):
    def __init__(self, profile_home):
        DI4SelfAdaptive.__init__(self, profile_home)

    def taskMiss(self, task):
        profile = task.profile
        if not isinstance(profile, DIProfile):
            print("in DI4StrategyB.taskMiss, the profile isn't a instance of DIProfile")
            sys.exit(-1)
        return profile.miss_Adaptive
