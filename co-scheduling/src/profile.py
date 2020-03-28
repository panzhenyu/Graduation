import sys

class Profile:
    def __init__(self):
        pass

class DIProfile(Profile):
    def __init__(self, miss_A=0, miss_B=0, miss_adaptive=0):
        Profile.__init__(self)
        
#        if miss_A < 0 or miss_B < 0 or miss_A > 1 or miss_B > 1:
#            print("miss_A or miss_B is invalid:\n"
#                    "\tmiss_A:{miss_A} miss_B:{missB}".format(miss_A=miss_A, miss_B=miss_B))
#            sys.exit(-1)
        self.miss_A = miss_A
        self.miss_B = miss_B
        self.miss_Adaptive = miss_adaptive

    def show(self):
        print(self.miss_A, self.miss_B)
