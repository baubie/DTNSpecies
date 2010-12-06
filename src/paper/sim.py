import neuron


class Simulation(object):

    def __init__(self, network, sim_time=100, dt=0.01):
        self.network = network
        self.sim_time = sim_time
        self.dt = dt
        self.stim = [[10,50]]

    def set_stim(self):
        self.clamps = []
        for s in self.stim:
            for c in self.network.cells:
                if self.network.cells[c]["stim"] == "Poisson":
                    pass
                elif self.network.cells[c]["stim"] == "IClamp":
                    for i in self.network.cells[c]["cells"]:
                        self.clamps.append(neuron.h.IClamp(0.5, sec=i.soma))
                        self.clamps[-1].amp = 0.01
                        self.clamps[-1].dur = 20
                        self.clamps[-1].delay = 10

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

