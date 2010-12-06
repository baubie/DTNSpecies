from sim import Simulation
import matplotlib.pyplot as plt
import network
import neuron

# Create our network
DTN = network.DTN()

# Play with the parameters
DTN.cells["IC"]["cells"][0].dendI.modifyGABAa(Cmax=1, Cdur=1, Alpha=5, Beta=0.18, Erev=-80, Prethresh=0, Deadtime=1)


def show(network):
    network.cells["IC"]["cells"][0].soma.show()
    for c in network.cells["DNLL"]["cells"]:
            c.soma.show()
    plt.show()


# Initialize the simulation with the network
s = Simulation(DTN)



# Run the simulation
s.stim_dur = 1
s.run()
show(DTN)

s.stim_dur = 10
s.run()
show(DTN)

s.stim_dur = 50
s.run()
show(DTN)


neuron.h.quit()
