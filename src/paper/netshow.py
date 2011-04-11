import sys
from copy import deepcopy

haveX = True

for arg in sys.argv:
    if arg == "NOX": haveX = False

if haveX:
    import matplotlib.pyplot as plt

def unique(seq):
    result = []
    for a in seq:
        if a not in result:
            result.append(a)
    return result

def frequency_unique(seq):
    unique_seq = unique(seq)
    vals = [0] * len(unique_seq)
    for i in range(len(unique_seq)):
        vals[i] = seq.count(unique_seq[i])
    return [unique_seq, vals]

def plot_mean_spikes(network, section):
    [uni, freq] = frequency_unique(network.savedparams)
    x_vals = unique([i[1] for i in uni]) 
    z_vals = unique([i[0] for i in uni])
    repeats = freq[0]
    max_y = 0
    
    y_vals_saved = []
    
    for z in range(len(z_vals)):
        y_vals = [0] * len(x_vals)
        for x in range(len(x_vals)):
            for s in range(len(network.savedcells)):
                if network.savedparams[s][1] == x_vals[x] and network.savedparams[s][0] == z_vals[z]:
                    for c in network.savedcells[s][section]:
                        y_vals[x] += float(len(c['rec_s'])) / repeats
        max_y = max(max_y, max(y_vals))

        if haveX: plt.plot(x_vals, y_vals, 'o-', label=str(z_vals[z]))
        y_vals_saved.append(y_vals)
    if haveX: plt.axis(ymin=0, ymax=max_y+0.1)
    if haveX: plt.legend()


def plot_voltage(network, section, param):
    count = 0
    for s in range(len(network.savedcells)):
        if network.savedparams[s] == param:
            for c in network.savedcells[s][section]:
                if haveX: plt.plot(c['rec_t'], c['rec_v'],label=str(param))
                if haveX: plt.axis(xmin=0, xmax=c['rec_t'][-1], ymin=-80, ymax=40)

def list_to_string(l):
    r = ""
    if type(l).__name__ == 'list':
        r  = str(l[0])
        for a in range(1,len(l)):
            r += "_"+str(l[a])
    else:
        r = str(l)

    return r

def save_mean_spikes(network, section, param, filename):
    f = open(filename, 'w')
    
    mean = {}
    stim = []
    for a in param:
        mean[list_to_string(a)] = {} 
    for a in param:
        for s in range(len(network.savedcells)): 
            if network.savedparams[s][0] == a:
                for c in network.savedcells[s][section]:
                    dur = network.savedparams[s][1]
                    if dur not in stim: stim.append(dur)
                    if dur not in mean[list_to_string(a)].keys(): mean[list_to_string(a)][dur] = []
                    mean[list_to_string(a)][dur].append(len(c['rec_s']))

    mean_sd = deepcopy(mean)

    # Calculate means
    for a in mean:
        for s in mean[a]:
            tmp = 0
            for e in mean[a][s]:
                tmp += e
            if len(mean[a][s]) > 0:
                mean[a][s] = float(tmp) / float(len(mean[a][s]))

    # Calculate standard deviation 
    for a in mean_sd:
        for s in mean_sd[a]:
            tmp = 0
            for e in mean_sd[a][s]:
                tmp += pow(e - mean[a][s],2.0) 
            if len(mean_sd[a][s]) > 1:
                mean_sd[a][s] = pow(tmp/(len(mean_sd[a][s])-1),0.5)
            else:
                mean_sd[a][s] = 0


    stim.sort()
    f.write('"Duration"')
    for a in param:
        f.write(',"'+list_to_string(a)+'",""')
    f.write("\n")
    for s in range(len(stim)):
        f.write(str(stim[s]))
        for a in param:
            if mean[list_to_string(a)][stim[s]] != []: f.write(","+str(mean[list_to_string(a)][stim[s]]))
            else: f.write(",-")
            f.write(","+str(mean_sd[list_to_string(a)][stim[s]]))
        f.write("\n")
    f.close()

def save_voltage(network, sections, param, filename):
    f = open(filename, 'w')
    f.write('"time"')
    for s in sections:
        f.write(',"'+s+'"')
    f.write("\n")
    rec_t = network.savedcells[0][sections[0]][0]['rec_t']

    for x in range(len(rec_t)):
        f.write(str(rec_t[x]))
        for s in range(len(network.savedcells)):
            if network.savedparams[s] == param:
                for section in sections:
                    # We are only saving the FIRST neuron here
                    f.write(','+str(network.savedcells[s][section][0]['rec_v'][x]))
        f.write("\n")
    f.close()

def save_current(network, section, receptors, param, filename):
    f = open(filename, 'w')
    f.write('"time"')
    for r in receptors:
        f.write(',"'+r+'"')
    f.write("\n")
    rec_t = network.savedcells[0][section][0]['rec_t']

    for x in range(len(rec_t)):
        f.write(str(rec_t[x]))
        for s in range(len(network.savedcells)):
            if network.savedparams[s] == param:
                for r in receptors:
                    # We are only saving the FIRST neuron here
                    f.write(','+str(network.savedcells[s][section][0]['rec_'+r+'i'][x]))
        f.write("\n")
    f.close()

def save_conductance(network, section, receptors, param, filename):
    f = open(filename, 'w')
    f.write('"time"')
    for r in receptors:
        f.write(',"'+r+'"')
    f.write("\n")
    rec_t = network.savedcells[0][section][0]['rec_t']

    for x in range(len(rec_t)):
        f.write(str(rec_t[x]))
        for s in range(len(network.savedcells)):
            if network.savedparams[s] == param:
                for r in receptors:
                    # We are only saving the FIRST neuron here
                    f.write(','+str(network.savedcells[s][section][0]['rec_'+r+'g'][x]))
        f.write("\n")
    f.close()


def save_spikes(network, section, param, filename, trials, diff=10):
    f = open(filename, 'w')

    count = {}
    for s in range(len(network.savedcells)): 
        if network.savedparams[s][0] == param:
            for c in network.savedcells[s][section]:
                dur = network.savedparams[s][1]
                if dur not in count.keys(): count[dur] = 0
                for spike in c['rec_s']:
                    out = str(spike)+","+str(dur+(float(count[dur])/trials)*diff*0.6+diff*0.3)
                    f.write(out+"\n")
                count[dur] += 1
    f.close()

def save_fsl(network, section, param, filename, trials):
    f = open(filename, 'w')
    
    fsl = {}
    stim = []
    for a in param:
        fsl[list_to_string(a)] = {}
    for a in param:
        for s in range(len(network.savedcells)): 
            if network.savedparams[s][0] == a:
                for c in network.savedcells[s][section]:
                    dur = network.savedparams[s][1]
                    if dur not in stim: stim.append(dur)
                    if dur not in fsl[list_to_string(a)].keys(): fsl[list_to_string(a)][dur] = []
                    if len(c['rec_s']) > 0: fsl[list_to_string(a)][dur].append(c['rec_s'][0])

    fsl_sd = deepcopy(fsl)

    # Calculate means
    for a in fsl:
        for s in fsl[a]:
            tmp = 0
            for e in fsl[a][s]:
                tmp += e
            if len(fsl[a][s]) > 0:
                fsl[a][s] = float(tmp) / float(len(fsl[a][s]))

    # Calculate standard deviation 
    for a in fsl_sd:
        for s in fsl_sd[a]:
            tmp = 0
            for e in fsl_sd[a][s]:
                tmp += pow(e - fsl[a][s],2.0) 
            if len(fsl_sd[a][s]) > 1:
                fsl_sd[a][s] = pow(tmp/(len(fsl_sd[a][s])-1),0.5)
            else:
                fsl_sd[a][s] = 0


    stim.sort()
    f.write('"Duration"')
    for a in param:
        f.write(',"'+str(a)+'",""')
    f.write("\n")
    for s in range(len(stim)):
        f.write(str(stim[s]))
        for a in param:
            if fsl[list_to_string(a)][stim[s]] != []: f.write(","+str(fsl[list_to_string(a)][stim[s]]))
            else: f.write(",-")
            f.write(","+str(fsl_sd[list_to_string(a)][stim[s]]))
        f.write("\n")
    f.close()

def plot_current(network, section, current, param):
    count = 0
    for s in range(len(network.savedcells)):
        if network.savedparams[s] == param:
            for c in network.savedcells[s][section]:
                if haveX: plt.plot(c['rec_t'], c['rec_'+current+'i'])


def plot_conductance(network, section, current, param):
    count = 0
    for s in range(len(network.savedcells)):
        if network.savedparams[s] == param:
            for c in network.savedcells[s][section]:
                if haveX: plt.plot(c['rec_t'], c['rec_'+current+'g'],label=str(param))


def subplot(row, col, num):
    if haveX: plt.subplot(row,col,num)

def show():
    if haveX: plt.show()

def legend():
    if haveX: plt.legend()
