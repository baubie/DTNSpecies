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
            row["BD"] = 3 #find_key(results,max(results.values()),middle=True)
            row["Max"] = max(results)
            tmpData.append(row)
            count = count + 1
        self.data = tmpData

        self.fieldnames = ["BD", "Max"]
        self.fieldnames.extend(reader.fieldnames)
            
        if (len(self.params()) > 0):
            self.isOpen = True
        return self.isOpen

    def params(self):
        if (len(self.data) == 0):
            return []
        for item in self.fieldnames:
        r.sort()
        return r        



