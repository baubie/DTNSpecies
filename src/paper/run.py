from sim import Simulation
import matplotlib.pyplot as plt
import network
import neuron

DTN = network.DTN()
s = Simulation(DTN)

# Play with the parameters
DTN.cells["IC"].dend.GABAa.Cmax = 1
DTN.cells["IC"].dend.GABAa.Cdur = 1
DTN.cells["IC"].dend.GABAa.Alpha = 5 
DTN.cells["IC"].dend.GABAa.Beta = 0.18
DTN.cells["IC"].dend.GABAa.Erev = -80
DTN.cells["IC"].dend.GABAa.Prethresh = 0
DTN.cells["IC"].dend.GABAa.Deadtime = 1

s.run()

DTN.cells["IC"].soma.show()
DTN.cells["IC"].dend.show()
DTN.cells["DNLL"].soma.show()
plt.show()
