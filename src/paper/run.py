from sim import Simulation
import cells

IC = cells.IC_Neuron()
s = Simulation(IC)
s.run()
