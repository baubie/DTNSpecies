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
    s[net].dt = 0.025


# Run the simulations
stims = [i for i in range(1,31,1)]
param = [(float(i)+1)*1000 for i in range(20)]
repeats = 15

total = len(stims)*len(param)*repeats
print "Running %d simulations..." % total
count = 0

for a in param: 
    for net in networks:
        networks["AC"].cells["IC"]["cells"][0].sec["soma"](0.5).pas.g = 1.0/a

    for d in stims*repeats:
        progress.update(count, total)
        count += 1
        for net in networks:
            s[net].stim_dur = d 
            s[net].run()
            key = [a,d] 
            networks[net].savecells([["IC","soma"]], key, spikes=True, voltage=False)

ns.plot_mean_spikes(networks["AC"], "IC-soma", "ac_mtc.dat")
ns.show()  # Comment out to just save the results to file

# Plot the results
if False:
    count = 0
    for a in param:
        for d in stims:
            count += 1
            key = [a,d]
            ns.subplot(len(param),len(stims),count)
            ns.plot_voltage(networks["C"], "IC-soma", key)
    progress.update(count, len(stims))
    ns.show()

neuron.h.quit()
