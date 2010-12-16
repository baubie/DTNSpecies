from sim import Simulation
import network
import neuron
import netshow as ns
import progress

# Create our network
networks = {}
#networks["C"] = network.DTN_Coincidence()
networks["AC"] = network.DTN_AntiCoincidence()


# Initialize the simulation with the network
s = {}
for net in networks:
    s[net] = Simulation(networks[net])
    s[net].verbose = False
    s[net].sim_time = 500
    s[net].dt = 0.025


# Run the simulations
stims = [i for i in range(1,31,1)]
param = [(float(i)+1)*1000 for i in range(20)]
repeats = 15


stims = [1, 10, 50]
param = [0.0033, 0.0066, 0.0132, 0.0264, 0.528]
repeats = 1


total = len(stims)*len(param)*repeats
print "Running %d simulations..." % total
count = 0

for a in param: 
    for net in networks:
        networks["AC"].cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(Beta=a, mg=0)
        networks["AC"].cells["IC"]["cells"][0].sec["dendI"].modifyGABAa(gmax=0)

    for d in stims*repeats:
        progress.update(count, total)
        count += 1
        for net in networks:
            s[net].stim_dur = d 
            s[net].run()
            key = [a,d] 
            networks[net].savecells([["IC","soma"],["IC","dendE"]], key, spikes=True, 
                                                                         NMDAi=True, 
                                                                         AMPAi=True, 
                                                                         GABAai=True, 
                                                                         NMDAg=True, 
                                                                         AMPAg=True, 
                                                                         GABAag=True, 
                                                                         voltage=True)

if False:
    ns.plot_mean_spikes(networks["AC"], "IC-soma", "ac_mtc.dat")
    ns.show()  # Comment out to just save the results to file

# Plot the results
if True:
    count = 0
    for a in param:
        for d in stims:
            count += 1
            key = [a,d]
            ns.subplot(len(param),len(stims),count)
            ns.plot_voltage(networks["AC"], "IC-soma", key)
    progress.update(count, len(stims))
    ns.show()

if True:
    count = 0
    for a in param:
        for d in stims:
            count += 1
            key = [a,d]
            ns.subplot(len(param),len(stims),count)
            ns.plot_conductance(networks["AC"], "IC-dendE", "NMDA", key)
            ns.plot_conductance(networks["AC"], "IC-dendE", "AMPA", key)
            ns.plot_conductance(networks["AC"], "IC-dendE", "GABAa", key)
    progress.update(count, len(stims))
    ns.show()

neuron.h.quit()
