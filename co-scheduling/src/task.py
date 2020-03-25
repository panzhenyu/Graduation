class Task:
    def __init__(self, name):
        self.name = str(name)

    def loadProfile(self, profile):
        self.profile = profile

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
