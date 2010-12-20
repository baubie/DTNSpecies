import simulations as sims
import neuron
import network
import sys
import netshow as ns

ShowSpikes = True
ShowVoltage = False

# Define Paramters
stims = [i for i in range(1,25,1)]
param = [i*0.01 for i in range(1,11,1)]
stims = [1, 5, 10]
param = [0.01, 0.1]
repeats = 1
sim_time = 100
netdef = network.DTN_AntiCoincidence
modify = sims.AC_NMDA_BETA






#####
# EVERY BELOW HERE SHOULD NOT NEED TO BE MODIFIED


# Setup and run simulations in parallel
pc = neuron.h.ParallelContext()
numProcs = int(pc.nhost())
ret = []
pc.runworker()
for i in range(numProcs):
    pc.submit(sims.run,netdef,modify,numProcs,i,stims,param,repeats,sim_time,ShowSpikes,ShowVoltage)
while pc.working():
    ret.append(pc.pyret())
pc.done()

# Combine all of the results together
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


# Plot the mean number of spikes
if ShowSpikes:
    ns.plot_mean_spikes(net, "IC-soma", "ac_nmda_beta.dat")
    ns.show()  # Comment out to just save the results to file

# Plot the voltage traces
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


