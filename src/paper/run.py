from sim import Simulation
import network
import neuron
import netshow as ns
import progress

# Create our network
DTN_AC = network.DTN_AntiCoincidence()
DTN_C = network.DTN_Coincidence()

# Play with the parameters
#DTN_AC.cells["IC"]["cells"][0].dendI.modifyGABAa(Cmax=1, Cdur=1, Alpha=5, Beta=0.18, Erev=-80, Prethresh=0, Deadtime=1)


# Initialize the simulation with the network
s_C = Simulation(DTN_C)
s_C.verbose = False
s_C.sim_time = 50
s_AC = Simulation(DTN_AC)
s_AC.verbose = False
s_AC.sim_time = 50


# Run the simulations
stims = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]*5
count = 0
for d in stims:
    progress.update(count, len(stims))
    count += 1
    s_C.stim_dur = d
    s_AC.stim_dur = d
    s_C.run()
    s_AC.run()
    DTN_C.savecells([["IC","soma"]], d, spikes=True,voltage=False)
    DTN_AC.savecells([["IC","soma"]], d, spikes=True,voltage=False)
progress.update(count, len(stims))

ns.plot_mean_spikes(DTN_C, "IC-soma")
ns.plot_mean_spikes(DTN_AC, "IC-soma")
ns.show()

neuron.h.quit()
