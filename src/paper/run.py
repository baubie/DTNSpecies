import simulations as sims
import neuron
import network
import sys
import netshow as ns


# Define Paramters
ShowSpikes = True
ShowVoltage = False

repeats = 5
sim_time = 500
netdef = network.DTN_Coincidence
modify = sims.C_SEARCH
[stims,param] = sims.C_SEARCH(None,None,True)
spike_filename = "c_mix_big.dat"
total = len(stims)*len(param)*repeats



#####
# EVERY BELOW HERE SHOULD NOT NEED TO BE MODIFIED


# Setup and run simulations in parallel
pc = neuron.h.ParallelContext()
numProcs = int(pc.nhost())
print "Running "+str(total)+ " simulations via "+str(numProcs)+" processes..."
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
    ns.plot_mean_spikes(net, "IC-soma", spike_filename)
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


