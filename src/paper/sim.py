import neuron


class Simulation(object):

    def __init__(self, network, sim_time=100, dt=0.01):
        self.network = network
        self.sim_time = sim_time
        self.dt = dt
        self.stim = [[10,50]]

    def set_stim(self):
        clamps = []
        for s in self.stim:
            for c in self.network.cells:
                if self.network.cells[c].getStim == True:
                    clamp = neuron.h.IClamp(self.network.cells[c].soma(0.5))
                    clamp.delay = s[0]
                    clamp.amp = 0.03
                    clamp.dur = s[1]-s[0]
                    clamps.append(clamp)

    def run(self,sim_time=None):
        self.set_stim()
        neuron.h.dt = self.dt
        neuron.h.celsius = 36
        neuron.h.finitialize(-55)
        neuron.init()
        if sim_time:
            neuron.run(sim_time)
        else:
            neuron.run(self.sim_time)

