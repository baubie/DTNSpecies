import csv

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

def find_key(dic, val, last=False, middle=False):
    found = [k for k, v in dic.iteritems() if v == val]
    pos = 0
    if (last):
        pos = -1
    if (middle):
        pos = int((len(found)-1)/2.0)

    print found
    print pos
    return found[pos]



class LogFile:

    def __init__(self):
        self.isOpen = False
        self.data = []

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
        for row in self.data:
            results = self.getresults(count)
            row["BD"] = find_key(results,max(results.values()),middle=True)
            row["Max"] = max(results.values())
            tmpData.append(row)
            count = count + 1
        self.data = tmpData
            
        if (len(self.params()) > 0):
            self.isOpen = True
        return self.isOpen

    def params(self):
        if (len(self.data) == 0):
            return []
        params = []
        for item in self.data[0].keys():
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
        raw = self.data[entryNum]
        r = {}
        for item in raw:
            if is_number(item):
                r[float(item)] = raw[item]
        return r



