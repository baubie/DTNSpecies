import simulations
import neuron
import sys
import netshow as ns

ShowSpikes = True
ShowVoltage = False

stims = [i for i in range(1,25,1)]
param = [i*0.01 for i in range(1,11,1)]
repeats = 15

pc = neuron.h.ParallelContext()
numProcs = int(pc.nhost())
pc.runworker()
ret = []
for i in range(numProcs):
    pc.submit(simulations.NMDA_BETA,numProcs,i,stims,param,repeats,ShowSpikes,ShowVoltage)
while pc.working():
    ret.append(pc.pyret())
pc.done()

savedparams = []
savedcells = []

for i in range(numProcs):
    for r in ret:
        if r[0] == i:
            savedparams += r[1]
            savedcells += r[2]

class Network:
    def __init__(self):
        savedparams = []
        savedcells = []

net = Network()
net.savedparams = savedparams
net.savedcells = savedcells


if ShowSpikes:
    ns.plot_mean_spikes(net, "IC-soma", "ac_nmda_beta.dat")
    ns.show()  # Comment out to just save the results to file

# Plot the results
if ShowVoltage:
    count = 0
    for a in param:
        for d in stims:
            count += 1
            key = [a,d]
            ns.subplot(len(param),len(stims),count)
            ns.plot_voltage(net, "IC-soma", key)
    ns.show()

neuron.h.quit()


