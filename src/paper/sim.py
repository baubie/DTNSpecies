import cells
import neuron
import matplotlib.pyplot as plt
import numpy as np

class Simulation(object):

    def __init__(self, cell, sim_time=100, dt=0.01):
        self.cell = cell
        self.sim_time = sim_time
        self.dt = dt


    def show(self):
        x = np.array(self.rec_t)
        y = np.array(self.cell.soma.rec_v)
        plt.plot(x,y)
        plt.xlabel("Time (ms)")
        plt.ylabel("Voltage (mV")
        plt.axis(ymin=-90, ymax=50)

    def run(self,sim_time=None):
        self.rec_t = neuron.h.Vector()
        self.rec_t.record(neuron.h._ref_t)
        neuron.h.dt = self.dt
        neuron.h.finitialize(self.cell.soma.E)
        neuron.init()
        if sim_time:
            neuron.run(sim_time)
        else:
            neuron.run(self.sim_time)

