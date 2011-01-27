import simulations as sims
import neuron
import network
import sys
import netshow as ns


# Define Paramters
ShowSpikes = True
ShowVoltage = False

netdef = network.DTN_CoincidenceSimple
modify = sims.C_DEFAULT
spike_filename = "c_default.dat"
tosave = [["IC","soma"],["MSO_ON","soma"],["MSO_OFF","soma"]]





#####
# EVERY BELOW HERE SHOULD NOT NEED TO BE MODIFIED

[repeats,sim_time,stims,param] = modify(None,None,None,True)
total = len(stims)*len(param)*repeats

# Setup and run simulations in parallel
pc = neuron.h.ParallelContext()
numProcs = int(pc.nhost())
if pc.id() == 0:
    print "Running "+str(total)+ " simulations via "+str(numProcs)+" processes..."
ret = []
pc.runworker()
for i in range(numProcs):
    pc.submit(sims.run,netdef,tosave,modify,numProcs,i,stims,param,repeats,sim_time,ShowSpikes,ShowVoltage)
while pc.working():
    ret.append(pc.pyret())
pc.done()
print "Simulation complete.  Post processing."

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
    for d in stims:
        count += 1
        ns.subplot(len(stims),1,count)
        for a in param:
            key = [a,d]
            ns.plot_voltage(net, "IC-soma", key)
            ns.plot_voltage(net, "MSO_ON-soma", key)
            ns.plot_voltage(net, "MSO_OFF-soma", key)
    ns.legend()
    ns.show()

neuron.h.quit()

