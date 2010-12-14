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
    s[net].sim_time = 100


# Run the simulations
stims = [i for i in range(1,20)]
param = [0.0,0.01,0.02,0.03,0.04,0.05]
repeats = 5

total = len(stims)*len(param)*repeats
count = 0

for a in param: 
    for net in networks:
        networks["AC"].cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=a)

    for d in stims*repeats:
        progress.update(count, total)
        count += 1
        for net in networks:
            s[net].stim_dur = d 
            s[net].run()
            key = [a,d] 
            networks[net].savecells([["IC","soma"]], key, spikes=True, voltage=True)

# Plot the results
'''
count = 0
for a in param:
    for d in stims:
        count += 1
        key = [a,d]
        ns.subplot(len(param),len(stims),count)
        ns.plot_voltage(networks["AC"], "IC-soma", key)


progress.update(count, len(stims))
ns.show()
'''

ns.plot_mean_spikes(networks["AC"], "IC-soma")
ns.show()

neuron.h.quit()
