class Task:
    def __init__(self, name):
        self.name = str(name)

    def loadProfile(self, profile):
        self.profile = profile

    def __eq__(self, _task):
        return _task.name == self.name

    def __lt__(self, _task):
        return self.name < _task.name

class TaskSet:
    def __init__(self):
        self.tasks = []

    def addTask(self, task):
        if isinstance(task, Task):
            self.tasks.append(task)

    def isEmpty(self):
        return len(self.tasks) == 0

    def length(self):
        return len(self.tasks)

    def getTaskList(self):
        return self.tasks

    def show(self):
        names = []
        for task in self.tasks:
            names.append(task.name)
        print(names)

    def __str__(self):
        sequence = ""
        self.tasks.sort()
        for task in self.tasks:
            sequence += task.name + " "
        return sequence[0: -1]

    def __eq__(self, _taskSet):
        set_len = len(self.tasks)
        if set_len != len(_taskSet.tasks):
            return False
        self.tasks.sort()
        _taskSet.tasks.sort()
        for i in range(set_len):
            if self.tasks[i] != _taskSet.tasks[i]:
                return False
        return True

    def __lt__(self, _taskSet):
        self.tasks.sort()
        _taskSet.tasks.sort()
        self_len, another_len = len(self.tasks), len(_taskSet.tasks)
        for i in range(min(self_len, another_len)):
            if self.tasks[i] < _taskSet.tasks[i]:
                return True
            elif self.tasks[i] > _taskSet.tasks[i]:
                return False
        if self_len < another_len:
           return True
        return False

