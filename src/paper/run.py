import simulations as sims
import neuron
import network
import sys
import netshow as ns


# Define Paramters
ShowMean = False
ShowVoltage = False
ShowSpikes = False
ShowConductance = False

SaveFSL = True
SaveMean = True
SaveSpikes = False

SaveVoltage = False
SaveConductance = False
SaveCurrent = False

netdef = network.DTN_CoincidenceSimple
modify = sims.C_INHIBITION

base_filename = "c_inhibition"

tosave = [["IC","soma"],["MSO_ON","soma"],["MSO_OFF","soma"]]
tosave = [["IC","soma"]]






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
    pc.submit(sims.run,netdef,tosave,modify,numProcs,i,stims,param,repeats,sim_time,SaveSpikes or ShowSpikes or ShowMean or SaveMean or SaveFSL,ShowVoltage or SaveVoltage,SaveConductance or ShowConductance,SaveCurrent)
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
if ShowMean:
    ns.plot_mean_spikes(net, "IC-soma")
    ns.show()

if SaveMean:
    ns.save_mean_spikes(net, "IC-soma", param, base_filename+"_mean.dat")

if SaveSpikes:
    for a in param:
        ns.save_spikes(net, "IC-soma", a, base_filename+"_spikes_"+ns.list_to_string(a)+".dat", repeats)

if SaveFSL:
    ns.save_fsl(net, "IC-soma", param, base_filename+"_fsl.dat", repeats)

if SaveVoltage:
    for d in stims:
        for a in param:
            key = [a,d]
            ns.save_voltage(net, ["IC-soma", "MSO_ON-soma", "MSO_OFF-soma"], key, base_filename+"_voltage_s_"+str(d)+"_a_"+str(a)+".dat")

if SaveConductance:
    for d in stims:
        for a in param:
            key = [a,d]
            ns.save_conductance(net, "IC-soma", ["AMPA","NMDA","GABAa"], key, base_filename+"_conductance_s_"+str(d)+"_a_"+str(a)+".dat")

if SaveCurrent:
    for d in stims:
        for a in param:
            key = [a,d]
            ns.save_current(net, "IC-soma", ["AMPA","NMDA","GABAa"], key, base_filename+"_current_s_"+str(d)+"_a_"+str(a)+".dat")




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

# Plot the condutance
if ShowConductance:
    count = 0
    for d in stims:
        count += 1
        ns.subplot(len(stims),1,count)
        for a in param:
            key = [a,d]
            ns.plot_conductance(net, "IC-soma", "GABAa", key)
    ns.legend()
    ns.show()



neuron.h.quit()

