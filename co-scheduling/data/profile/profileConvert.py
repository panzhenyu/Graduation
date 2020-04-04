import os, xlrd

if __name__ == "__main__":
	wb = xlrd.open_workbook("data.xlsx")
	sheet = wb.sheet_by_name("Miss_Cycle")
	for i in range(1, sheet.nrows):
		values = sheet.row_values(i)
		if isinstance(values[0], float):
			taskName = str(int(values[0]))
		else:
			taskName = values[0]
		folderPath = "./profile/" + taskName
		os.mkdir(folderPath)

		di = open(folderPath + "/DI", "w")
		info = ""
		for j in range(1, 4):
			info += str(values[j]) + " "
		info = info[:-1] + '\n'
		di.write(info)
		di.close()
