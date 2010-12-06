from sim import Simulation
import matplotlib.pyplot as plt
import network
import neuron

DTN = network.DTN()
s = Simulation(DTN)


# Play with the parameters
DTN.cells["IC"]["cells"][0].dend.modifyGABAa(Cmax=1, Cdur=1, Alpha=5, Beta=0.18, Erev=-80, Prethresh=0, Deadtime=1)

s.run()

DTN.cells["IC"]["cells"][0].soma.show()
DTN.cells["IC"]["cells"][0].dend.show()

for c in DTN.cells["DNLL"]["cells"]:
    c.soma.show()

#plt.show()
