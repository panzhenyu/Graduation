from utils import *
from config.path import *
from src.algorithm import DI4CompareImproveSelect, DI4NonStrategyImproveSelect
import sys, xlwt

# this function return two taskSet: strategyA, strategyB
# tasks working well in A strategy
# tasks working well in B strategy
def loadTaskSet():
    strategyA = [
        "502", "505", "519", "520",
        "620", "625", "638", "calculix",
        "gromacs", "lbm"
    ]
    strategyB = [
        "557", "hmmer", "libquantum", "mcf",
        "soplex"
    ]
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
def predictedValues(taskNames, DITaskObjs, methodList):
    result = {}
    for method in methodList:
        schedule, tasks = method(""), []
        for name in taskNames:
            tasks.append(DITaskObjs[name])
        predicted = schedule.predict(tasks)
        result[method.name] = predicted
    return result

# result is in dict format:
# result = {taskNameSeq: predictedValues}
# predictedValues = {methodName: value, ...}
def test4Strategy(strategy, DITaskObjs, methodList):
    result = {}
    comb = sub(strategy, 2)
    for taskNames in comb:
        taskNameSeq = getTaskNameSeq(taskNames)
        predicted = predictedValues(taskNames, DITaskObjs, methodList)
        result[taskNameSeq] = predicted
    return result

def test4AB(strategyA, strategyB, DITaskObjs, methodList):
    result = {}
    for taskA in strategyA:
        for taskB in strategyB:
            taskNames = [taskA, taskB]
            taskNameSeq = getTaskNameSeq(taskNames)
            predicted = predictedValues(taskNames, DITaskObjs, methodList)
            result[taskNameSeq] = predicted
    return result

def calcRealValue(taskNameSeq, corunResult):
        taskSet = corunResult[taskNameSeq]
        misses, cycles = 0, 0
        for taskAttr in taskSet.values():
            misses += taskAttr["miss"]
            cycles += taskAttr["cycle"]
        return misses / cycles

# save result in methodNames order
def saveSheet(sheet, result, corunResult, methodNames):
    headLine = ['seq', 'taskNameSeq', 'real']
    for methodName in methodNames:
        headLine.append(methodName)
    for i in range(len(headLine)):
        sheet.write(0, i, headLine[i])

    seq = 1
    for taskNameSeq in result.keys():
        predictedValues = result[taskNameSeq]
        real = calcRealValue(taskNameSeq, corunResult)
        line = [seq, taskNameSeq, real]
        for methodName in methodNames:
            line.append(predictedValues[methodName])
        for i in range(len(line)):
            sheet.write(seq, i, line[i])
        seq += 1

def saveExcel(output, resultA, resultB, resultAB, corunResult, methodList):
    methodNames = []
    for method in methodList:
        methodNames.append(method.name)

    workbook = xlwt.Workbook(encoding='utf-8')
    sheetA = workbook.add_sheet("A")
    sheetB = workbook.add_sheet("B")
    sheetAB = workbook.add_sheet("AB")

    saveSheet(sheetA, resultA, corunResult, methodNames)
    saveSheet(sheetB, resultB, corunResult, methodNames)
    saveSheet(sheetAB, resultAB, corunResult, methodNames)

    workbook.save(output)
    

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("usage: python3 Check4PredictMethod CorunResultPath output")
        sys.exit(-1)
    corunResultPath, output = sys.argv[1], sys.argv[2]
    methodList = [DI4CompareImproveSelect, DI4NonStrategyImproveSelect]

    nameIdMap = loadNameIdMap(TASKID_PATH)
    idPhaseMap = loadIdPhaseMap(PHASE_PATH)
    strategyA, strategyB = loadTaskSet()
    allTask = strategyA[:]
    allTask.extend(strategyB)
    DITaskObjs = buildDITaskObject(allTask, PROFILE_HOME)
    corunResult = loadCorunResult(corunResultPath)
    
    resultA = test4Strategy(strategyA, DITaskObjs, methodList)
    resultB = test4Strategy(strategyB, DITaskObjs, methodList)
    resultAB = test4AB(strategyA, strategyB, DITaskObjs, methodList)

    saveExcel(output, resultA, resultB, resultAB, corunResult, methodList)
