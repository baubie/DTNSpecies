from sim import Simulation
import matplotlib.pyplot as plt
import network
import neuron

# Create our network
DTN = network.DTN()

# Play with the parameters
DTN.cells["IC"]["cells"][0].dendI.modifyGABAa(Cmax=1, Cdur=1, Alpha=5, Beta=0.18, Erev=-80, Prethresh=0, Deadtime=1)


def show(network):
    plt.subplot(3,1,1)
    network.cells["IC"]["cells"][0].soma.show("soma")
    network.cells["IC"]["cells"][0].dendE.show("dendE")
    network.cells["IC"]["cells"][0].dendI.show("dendI")
    plt.legend()
    plt.subplot(3,1,2)
    for c in network.cells["MSO"]["cells"]:
        c.soma.show()
    plt.subplot(3,1,3)
    for c in network.cells["DNLL"]["cells"]:
        c.soma.show()
    plt.show()


# Initialize the simulation with the network
s = Simulation(DTN)



# Run the simulations
for d in [1,2,3,4,5]:
    s.stim_dur = d
    s.sim_time = 50
    s.run()
    show(DTN)

neuron.h.quit()
