import matplotlib.pyplot as plt

def unique(seq, idfun=None):
    if idfun is None:
        def idfun(x): return x
    seen = {}
    result = []
    for item in seq:
        marker = idfun(item)
        if marker in seen: continue
        seen[marker] = 1
        result.append(item)
    return result

def frequency_unique(seq):
    unique_seq = unique(seq)
    vals = [0] * len(unique_seq)
    for i in range(len(unique_seq)):
        vals[i] = seq.count(unique_seq[i])
    return [unique_seq, vals]

def plot_mean_spikes(network, section):
    vals = []
    [params, freq] = frequency_unique(network.savedparams)
    vals = [0] * len(params)
    for p in range(len(params)):
        for s in range(len(network.savedcells)):
            if network.savedparams[s] == params[p]:
                for c in network.savedcells[s][section]:
                    vals[p] += float(len(c['rec_s'])) / float(freq[p])

    plt.plot(params, vals, 'o-')
    plt.axis(ymin=0, ymax=max(vals)+0.1)


def show_voltage(network, cells):
    # Example Usage:    
    # ns.show_voltage(DTN_C, ["MSO_ON", "MSO_OFF", "DNLL"])
    plt.subplot(len(cells)+1,1,1)
    network.cells["IC"]["cells"][0].sec["soma"].show("soma")

    count = 1
    for pop in cells:
        count = count + 1
        plt.subplot(len(cells)+1,1,count)
        for c in network.cells[pop]["cells"]:
            c.sec["soma"].show()

    plt.show()

def show():
    plt.show()
