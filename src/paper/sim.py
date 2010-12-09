import neuron


class Simulation(object):

    def __init__(self, network, sim_time=100, dt=0.01, delay=10):
        self.network = network
        self.sim_time = sim_time
        self.dt = dt
        self.delay = delay
        self.stim = [[10,50]]
        self.clamps = []
        self.poisson = None
        self.poisson = neuron.h.Random(102)
        self.poisson.poisson(0.05)
        self.stim_dur = 10

    def set_stim(self):
        print "...Creating Stimuli"
        self.clamps = []
        for s in self.stim:
            for c in self.network.cells:
                if self.network.cells[c]["stim"] == "Poisson":
                    for i in self.network.cells[c]["cells"]:
                        self.clamps.append(neuron.h.IClamp(0.5, sec=i.soma))
                        self.poisson.play(self.clamps[-1]._ref_amp)
                        if self.network.cells[c]["type"] == "Sustained":
                            self.clamps[-1].dur = self.stim_dur
                        elif self.network.cells[c]["type"] == "Onset":
                            self.clamps[-1].dur = 2
                        self.clamps[-1].delay = self.delay+self.network.cells[c]["delay"]

                elif self.network.cells[c]["stim"] == "IClamp":
                    for i in self.network.cells[c]["cells"]:
                        self.clamps.append(neuron.h.IClamp(0.5, sec=i.soma))
                        self.clamps[-1].amp = 0.01
                        self.clamps[-1].dur = self.stim_dur
                        self.clamps[-1].delay = self.delay+self.network.cells[c]["delay"]

    def run(self):
        print "Initializing Simulation"
        self.set_stim()
        neuron.h.dt = self.dt
        neuron.h.celsius = 37
        neuron.h.finitialize(-55)
        neuron.init()
        print "...Running Simulation"
        neuron.run(self.sim_time)
        print "...Simulation Complete\n"

