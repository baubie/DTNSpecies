import sys

if len(sys.argv) < 2:
    print "Error: Please provide a file to analze."
    quit()

import csv

# Capture raw data
filename = sys.argv[1]
data = csv.reader(open(filename, 'rb'), delimiter=',')
x = []
y = []
y_names = []
first = True
for d in data:
    if first:
        first = False
        y_names = d[1:]
        y = [[] for i in range(len(y_names))]
    else:
        x.append(float(d[0]))
        for i in range(len(y_names)):
            y[i].append(float(d[i+1])) 


tuning_widths = []
best_duration = []
most_spikes = []
for series in range(len(y)):
    best_duration.append(x[y[series].index(max(y[series]))])
    most_spikes.append(max(y[series]))
    max_pos = y[series].index(max(y[series]))
    half = max(y[series]) / 2.0
    half_pos = -1
    width = 999

    if half in y[series]:
        found = False
        for i in range(max_pos, len(y[series])):
            if found == True:
                if y[series][i] != half: 
                    half_pos = i-1
                    break
            if found == False:
                if y[series][i] == half: found = True

        width = x[half_pos] - x[max_pos] 
    else:
        for i in range(max_pos, len(y[series])):
            if y[series][i] < half:
                m = (y[series][i]-y[series][i-1])/(x[i]-x[i-1])
                distance = (half-y[series][i-1]) / m
                width = x[i-1]+distance - x[max_pos]
                break

    tuning_widths.append(width)


f = open("prop_"+filename, 'w')
f.write('"Series","Width","Best Duration","Peak Spikes"\n')
for i in range(len(y_names)):
    f.write("%s," % y_names[i])
    f.write("%s," % tuning_widths[i])
    f.write("%s," % best_duration[i])
    f.write("%s" % most_spikes[i])
    f.write("\n")
f.close()
