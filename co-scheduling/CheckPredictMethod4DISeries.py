from utils import *
from config.path import *
from src.algorithm import DI4CompareImproveSelect, DI4NonStrategyImproveSelect
import sys, xlwt

# this function return two taskSet: strategyA, strategyB
# tasks working well in A strategy
# tasks working well in B strategy
def divTaskSetAsDIProfile(taskNames):
    taskObjs = buildDITaskObject(taskNames, CO_SCHEDULING_HOME + "/profile")
    strategyA, strategyB = [], []
    for name in taskNames:
        profile = taskObjs[name].profile
        if profile.miss_A < profile.miss_B:
            strategyA.append(name)
        else:
            strategyB.append(name)
    del taskObjs
    return strategyA, strategyB

def getTaskNameSeq(taskNames):
    taskNames.sort()
    taskNameSeq = ""
    for name in taskNames:
        taskNameSeq += name + ' '
    taskNameSeq = taskNameSeq[:-1]
    return taskNameSeq

# taskNames is a list of taskName
# return a dict:
# result = {methodName: value, ...}
def calcPredictedValues(taskNames, DITaskObjs, methodList):
    result = {}
    for method in methodList:
        schedule, tasks = method(""), []
        for name in taskNames:
            tasks.append(DITaskObjs[name])
        predicted = schedule.predict(tasks)
        result[method.name] = predicted
    return result

# note that the cycle must be average cycle
def calcRealValue(taskNameSeq, corunResult):
    taskSet = corunResult[taskNameSeq]
    missCycle_sum = 0
    for taskAttr in taskSet.values():
        miss = taskAttr["miss"]
        cycle = taskAttr["cycle"]
        missCycle_sum += miss / float(cycle)
    return missCycle_sum

# comb = [taskNames, ...]
# taskNames = [taskName, ...]
# result is in dict format:
# result = {taskNameSeq: values}
# values = {"predicted": predictedValues, "real": realValue}
# predictedValues = {methodName: value, ...}
# realValue = float
def getAllValues(combs, DITaskObjs, methodList, corunResult):
    result = {}
    for taskNames in combs:
        taskNameSeq = getTaskNameSeq(taskNames)
        predicted = calcPredictedValues(taskNames, DITaskObjs, methodList)
        real = calcRealValue(taskNameSeq, corunResult)
        result[taskNameSeq] = {}
        result[taskNameSeq]['predicted'] = predicted
        result[taskNameSeq]['real'] = real
    return result

# save result in methodNames order
def saveSheet(sheet, result, methodNames):
    headLine = ['seq', 'taskNameSeq', 'real']
    for methodName in methodNames:
        headLine.append(methodName)
    for i in range(len(headLine)):
        sheet.write(0, i, headLine[i])

    seq = 1
    for taskNameSeq in result.keys():
        values = result[taskNameSeq]
        predictedValues, real = values['predicted'], values['real']
        line = [seq, taskNameSeq, real]
        for methodName in methodNames:
            line.append(predictedValues[methodName])
        for i in range(len(line)):
            sheet.write(seq, i, line[i])
        seq += 1

def merge2(_list1, _list2):
    result = []
    for elem1 in _list1:
        for elem2 in _list2:
            if elem1 != elem2:
                result.append([elem1, elem2])
    return result

def merge3(_list1, _list2, _list3):
    result = []
    for elem1 in _list1:
        for elem2 in _list2:
            if elem1 == elem2:
                continue
            for elem3 in _list3:
                if elem3 != elem1 and elem3 != elem2:
                    result.append([elem1, elem2, elem3])
    return result

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: python3 thisfile output")
        sys.exit(-1)
    output = sys.argv[1]
    methodList = [DI4CompareImproveSelect, DI4NonStrategyImproveSelect]

    nameIdMap = loadNameIdMap(TASKID_PATH)
    idPhaseMap = loadIdPhaseMap(PHASE_PATH)
    taskNames = getAllTask(AUTOTEST_TASKSET)
    strategyA, strategyB = divTaskSetAsDIProfile(taskNames)
    allTask = strategyA[:]
    allTask.extend(strategyB)
    DITaskObjs = buildDITaskObject(allTask, PROFILE_HOME)
    corunResult = loadCorunResult(AUTOTEST_CORUNOUT)
    methodNames = [x.name for x in methodList]
    print(strategyA)
    print(strategyB)
    results = {
        "AA": getAllValues(sub(strategyA, 2), DITaskObjs, methodList, corunResult),
        "BB": getAllValues(sub(strategyB, 2), DITaskObjs, methodList, corunResult),
        "AB": getAllValues(merge2(strategyA, strategyB), DITaskObjs, methodList, corunResult),
        "AAA": getAllValues(sub(strategyA, 3), DITaskObjs, methodList, corunResult),
        "BBB": getAllValues(sub(strategyB, 3), DITaskObjs, methodList, corunResult),
        "AAB": getAllValues(merge3(strategyA, strategyA, strategyB), DITaskObjs, methodList, corunResult),
        "ABB": getAllValues(merge3(strategyA, strategyB, strategyB), DITaskObjs, methodList, corunResult)
    }

    workbook = xlwt.Workbook(encoding='utf-8')
    for sheetName in results.keys():
        data = results[sheetName]
        sheet = workbook.add_sheet(sheetName)
        saveSheet(sheet, data, methodNames)
    workbook.save(output)

