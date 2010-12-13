from sim import Simulation
import network
import neuron
import netshow as ns

# Create our network
DTN_AC = network.DTN_AntiCoincidence()
DTN_C = network.DTN_Coincidence()

# Play with the parameters
#DTN_AC.cells["IC"]["cells"][0].dendI.modifyGABAa(Cmax=1, Cdur=1, Alpha=5, Beta=0.18, Erev=-80, Prethresh=0, Deadtime=1)


# Initialize the simulation with the network
s = Simulation(DTN_C)


# Run the simulations
stims = [1,2,3,4,5,6,7,8,9,10]
for d in stims:
    s.stim_dur = d
    s.sim_time = 40
    s.run()
    DTN_C.savecells([["IC","soma"]],spikes=True,voltage=False)
#    ns.show_voltage(DTN_C, ["MSO_ON", "MSO_OFF", "DNLL"])

ns.show_mean_spikes(DTN_C, "IC-soma", stims)

neuron.h.quit()
