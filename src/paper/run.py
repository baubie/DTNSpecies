from sim import Simulation
import matplotlib.pyplot as plt
import network
import neuron

# Create our network
DTN = network.DTN()

# Play with the parameters
DTN.cells["IC"]["cells"][0].dend.modifyGABAa(Cmax=1, Cdur=1, Alpha=5, Beta=0.18, Erev=-80, Prethresh=0, Deadtime=1)


# Initialize the simulation with the network
s = Simulation(DTN)

# Run the simulation
s.run()


# Display the simulation results
DTN.cells["IC"]["cells"][0].soma.show()
for c in DTN.cells["DNLL"]["cells"]:
    c.soma.show()
plt.show()
