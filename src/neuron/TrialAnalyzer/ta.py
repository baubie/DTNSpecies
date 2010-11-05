import sys
import csv
from math import floor


if len(sys.argv) == 1:
    print ("Error: Please provide an input filename.")
    sys.exit()

# Valid modes are:
#   means   - print out mean spikes for GLE
#   spikes  - print out spike times for GLE
#   timeseries  - print out time series with data series

mode = "means"

if len(sys.argv) == 3:
    mode = sys.argv[2]

datafile = csv.reader(open(sys.argv[1]), delimiter=',')

trial_delay = -1
trial_dur = -1
trials = -1
minTrial = -1
dTrial = -1
dt = 0.025


runs = 0


for row in datafile:
    if (trial_delay == -1):
        trial_delay = int(row[0])
        trial_dur = int(row[1])
        trials = int(row[2])
        minTrial = float(row[3])
        dTrial = float(row[4])
        num_spikes = [0 for x in range(trials)]
        spikes = [ [] for x in range(trials)] 
    else:
        runs += 1
        if mode == "means":
            for spike in row:
                if spike != "":
                    i = floor((float(spike)-trial_delay)/trial_dur)
                    num_spikes[i] += + 1.0

        if mode == "spikes":
            for spike in row:
                for t in range(trials):
                    spikes[t].append([])
                if spike != "":
                    i = floor((float(spike)-trial_delay)/trial_dur)
                    time = float(spike)-i*trial_dur-trial_delay
                    spikes[i][runs-1].append(time)

        if mode == "timeseries":
            if len(row) == 1:
                print((runs-1)*dt, row[0])


if mode == "means":
    num_spikes = [i/runs for i in num_spikes]
    for stim in range(trials): 
        print (minTrial+stim*dTrial, num_spikes[stim])

if mode == "spikes":
    dt = (dTrial/runs)*0.75
    tCount = 0
    for trial in spikes:
        y = 0
        for run in trial:
            y += dt
            for spike in run:
                print(spike, minTrial+tCount*dTrial+y)
        tCount += 1




