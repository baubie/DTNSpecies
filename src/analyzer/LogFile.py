import csv

def is_number(s):
    try: 
        float(s)
        return True
    except ValueError:
        return False


class LogFile:

    def __init__(self):
        self.isOpen = False
        self.data = []
        self.fieldnames = []

    def open(self, filename):
        # Open the file and parse it
        f = open(filename).readlines()
        while len(f) > 0:
            l = f.pop(0)
            if (l[0:5] == "====="):
                break
        reader = csv.DictReader(f)
        self.data = [row for row in reader]

        # Compute some data
        tmpData = []
        count = 0
        self.fieldnames = reader.fieldnames
        for row in self.data:
            results = self.getresults(count)
            row["BD"] = self.BD(count)
            row["Max"] = max(results)
            row["50% Width"] = self.fiftywidth(count)
            row["Sharpness"] = self.sharpness(count)
            tmpData.append(row)
            count = count + 1
        self.data = tmpData
        
        self.fieldnames = ["BD", "Max", "50% Width", "Sharpness"]
        self.fieldnames.extend(reader.fieldnames)
            
        if (len(self.params()) > 0):
            self.isOpen = True
        return self.isOpen

    def fiftywidth(self, entrynum):
        results = self.getresults(entrynum)
        maxSpikes = max(results)
        BD = self.BD(entrynum)
        durs = self.getdurs()
        start = -1
        end = -1
        for i in range(len(results)):
            if (start == -1 and results[i] >= maxSpikes/2):
                if i > 0:
                    m = (results[i]-results[i-1])/(durs[i]-durs[i-1])
                    start = (maxSpikes/2 - results[i-1])/m+durs[i-1]
                else:
                    start = durs[0]
            elif (end == -1 and start > -1 and durs[i] > BD and results[i] <= maxSpikes/2):
                if i > 0:
                    m = (results[i]-results[i-1])/(durs[i]-durs[i-1])
                    if m == 0:
                        m = 1
                    end = (maxSpikes/2 - results[i-1])/m+durs[i-1]
                else:
                    end = durs[0]
        return end-start

    def sharpness(self, entrynum):
        BD = float(self.BD(entrynum))
        width = float(self.fiftywidth(entrynum))
        if width == 0:
            width = 1
        return BD/width

    def BD(self, entrynum):
        results = self.getresults(entrynum)
        durs = self.getdurs()
        maxSpikes = max(results)
        start = -1
        end = -1
        mid = -1
        for i in range(len(results)): 
            if (results[i] == maxSpikes):
                if start == -1:
                    start = i
                end = i
                mid = start + (end-start)/2
        return durs[start]


    def params(self):
        if (len(self.data) == 0):
            return []
        params = []
        for item in self.fieldnames:
            if is_number(item) == False:
                params.append(item)
        return params

    def networkdefs(self):
        if (len(self.data) == 0):
            return []
        r = []
        count = 0
        for entry in self.data:
            row = [count]
            for param in self.params():
                if (is_number(entry[param])):
                    row.append(float(entry[param]))
                else:
                    row.append("-")
            if (count > 0): # First row is just sample data
                r.append(row)
            count = count + 1
        return r

    def getresults(self, entryNum):
        r = []
        for item in self.fieldnames:
            if is_number(item):
                r.append(float(self.data[entryNum][item]))
        return r

    def getdurs(self):
        r = []
        for item in self.fieldnames:
            if is_number(item):
                r.append(float(item))
        r.sort()
        return r
                

