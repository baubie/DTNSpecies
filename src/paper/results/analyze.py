import sys
from subprocess import call

if len(sys.argv) < 2:
    print "Error: Please provide a file to analze."
    quit()

import csv

# Capture raw data
filename = sys.argv[1]
data = csv.reader(open(filename+".dat", 'rb'), delimiter=',')
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


f = open(filename+".analyze", 'w')
f.write('"Series","Width","Best Duration","Peak Spikes"\n')
for i in range(len(y_names)):
    f.write("%s," % y_names[i])
    f.write("%s," % tuning_widths[i])
    f.write("%s," % best_duration[i])
    f.write("%s" % most_spikes[i])
    f.write("\n")
f.close()


def color(i, total):
    points = [ [0.0, [1,0,0]],
               [0.5, [0,0,1]],
               [1.0, [0,0,0]],
             ]
    pos = float(i)/float(total)
    col = []
    last = points[0]
    for p in points:
        if p[0] == pos: 
            col = p[1]
            break
        if pos < p[0]: 
            col = [last[1][i]+(p[1][i]-last[1][i])*(p[0]-pos)/(p[0]-last[0]) for i in range(3)]
            break
        last = p
    return 'rgb('+str(col[0])+','+str(col[1])+','+str(col[2])+')'




# Output GLE file
f = open(filename+".gle", "w")
f.write('size 8 3\n')
f.write('set font psh\n')
f.write('set hei 0.15\n')
f.write('set lwidth 0.02\n')
f.write('begin graph\n')
f.write('    size 4 3\n')
f.write('    scale auto\n')
f.write('    xtitle "Stimulus Duration (ms)"\n')
f.write('    ytitle "Mean Spikes per Trial"\n')
f.write('    x2axis off\n')
f.write('    y2axis off\n')
f.write('    data "'+filename+'.dat"\n')
f.write('    xticks length -0.05\n')
f.write('    yticks length -0.05\n')
for i in range(len(y)):
    f.write('    d'+str(int(i+1))+' line color '+color(i,len(y))+'\n')
f.write('    key compact nobox\n')
f.write('end graph\n')
f.write('amove 4 0\n')
f.write('begin graph\n')
f.write('    size 4 3\n')
f.write('    scale auto\n')
f.write('    xtitle " (ms)"\n')
f.write('    ytitle "Width (ms) / Number of Spikes"\n')
f.write('    x2axis off\n')
f.write('    y2axis off\n')
f.write('    data "'+filename+'.analyze"\n')
f.write('    xticks length -0.05\n')
f.write('    yticks length -0.05\n')
f.write('    let d4 = x, d1/d3\n')
f.write('    d1 line marker fcircle msize 0.1 color rgb(1,0,0)\n')
f.write('    d2 line marker fsquare msize 0.1 color rgb(0,1,0)\n')
f.write('    d3 line marker fdiamond msize 0.1 color rgb(0,0,1)\n')
f.write('    d4 line marker ftriangle msize 0.1 color rgb(0,0,0) key "Norm Width"\n')
f.write('    key compact nobox pos br\n')
f.write('end graph\n')
f.close()


# View results in QGLE
call(["qgle", filename+".gle"])
