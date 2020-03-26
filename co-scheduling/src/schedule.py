from .task import *

class Schedule:
    def __init__(self):
        self.taskSets = []

    def addTaskSet(self, taskSet):
        if isinstance(taskSet, TaskSet):
            self.taskSets.append(taskSet)
        else:
            print("in Schedule.addTaskSet, the taskSet isn't a instance of TaskSet")

    def save(self, filename):
        pass

    def show(self):
        for t_set in self.taskSets:
            t_set.show()

    def __str__(self):
        s = ""
        for t_set in self.taskSets:
            s += str(t_set) + "\n"
        return s[0: -1]
