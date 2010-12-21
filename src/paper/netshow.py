import sys

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

def plot_mean_spikes(network, section, filename=None):
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

    if filename != None:
        f = open(filename, 'w')
        f.write('"x-axis"')
        for z in range(len(z_vals)):
            f.write(",\"%s\"" % str(z_vals[z]))
        f.write("\n")
        for x in range(len(x_vals)):
            f.write(str(x_vals[x]))
            for z in range(len(z_vals)):
                f.write(",%s" % str(y_vals_saved[z][x]))
            f.write("\n")
        f.close()

def plot_voltage(network, section, param):
    count = 0
    for s in range(len(network.savedcells)):
        if network.savedparams[s] == param:
            for c in network.savedcells[s][section]:
                if haveX: plt.plot(c['rec_t'], c['rec_v'])
                if haveX: plt.axis(xmin=0, xmax=c['rec_t'][-1], ymin=-80, ymax=40)

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
                if haveX: plt.plot(c['rec_t'], c['rec_'+current+'g'])


def subplot(row, col, num):
    if haveX: plt.subplot(row,col,num)

def show():
    if haveX: plt.show()
