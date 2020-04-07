import sys, json, xlwt

def loadPureMetrics(json_file):
    fp = open(json_file, "r")
    content = fp.read()
    data = json.loads(content)
    fp.close()
    return data

# result is in dict format:
# result = {algorithmName: metricObj, ...}
# metricObj = {metricName: metrics, ...}
# metrics = [metricValue, ...]
def collectMetrics(data, collectedMetrics):
    result = {}
    for algorithmName in data[0].keys():
        result[algorithmName] = {}
        for metricName in collectedMetrics:
            result[algorithmName][metricName] = []
    for scheduleObj in data:
        for algorithmName in scheduleObj.keys():
            metrics = scheduleObj[algorithmName]
            for metricName in collectedMetrics:
                result[algorithmName][metricName].append(metrics[metricName])
    return result

def saveExcel(data, output_file):
    wb = xlwt.Workbook()
    sheet = wb.add_sheet("1")

    algorithmNames = list(data.keys())
    metricNames = list(data[algorithmNames[0]].keys())
    sampleNum = len(data[algorithmNames[0]][metricNames[0]])
    algorithmNum, metricNum = len(algorithmNames), len(metricNames)

    s = 1
    for i in range(algorithmNum):
        sheet.write(0, s, algorithmNames[i])
        s += metricNum
    s = 1
    for j in range(algorithmNum):
        for i in range(metricNum):
            sheet.write(1, s, metricNames[i])
            s += 1

    metricLists= []
    for algorithmName in algorithmNames:
        for metricName in metricNames:
            metricLists.append(data[algorithmName][metricName])
    for i in range(sampleNum):
        column = 1
        sheet.write(i+2, 0, i)
        for metricList in metricLists:
            sheet.write(i+2, column, metricList[i])
            column += 1
    
    wb.save(output_file)

if __name__ == "__main__":
    argc = len(sys.argv)
    if argc != 3:
        print("usage: python3 PureMetric2Excel.py json_file output_file")
        sys.exit(-1)
    jsonFile = sys.argv[1]
    outputFile = sys.argv[2]
    collectedMetrics = ["IPC", "speedup", "unfairness", "fairness"]

    raw_data = loadPureMetrics(jsonFile)
    if len(raw_data) == 0:
        sys.exit(-1)
    final = collectMetrics(raw_data, collectedMetrics)
    saveExcel(final, outputFile)
