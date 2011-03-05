import neuron


class Simulation(object):

    def __init__(self, network, amplitude=0.10, randomseed=100, sim_time=200, dt=0.01, delay=10):
        self.network = network
        self.sim_time = sim_time
        self.dt = dt
        self.delay = delay
        self.stim = [[10,50]]
        self.clamps = []
        self.poisson = None
        self.poisson = neuron.h.Random(randomseed)
        index = self.poisson.MCellRan4(randomseed)
        self.poisson.poisson(amplitude)
        self.stim_dur = 10
        self.verbose = True

    def set_amplitude(self, amplitude):
        self.poisson.poisson(amplitude)

    def set_stim(self):
        if self.verbose: print "...Creating Stimuli"
        self.clamps = []
        for s in self.stim:
            for c in self.network.cells:
                if self.network.cells[c]["stim"] == "Poisson":
                    for i in self.network.cells[c]["cells"]:
                        self.clamps.append(neuron.h.IClamp(0.5, sec=i.sec["soma"]))
                        self.poisson.play(self.clamps[-1]._ref_amp)
                        if self.network.cells[c]["type"] == "Sustained":
                            self.clamps[-1].delay = self.delay+self.network.cells[c]["delay"]
                            self.clamps[-1].dur = max([self.stim_dur, self.network.cells[c]["mindur"]])
                        elif self.network.cells[c]["type"] == "Onset":
                            self.clamps[-1].delay = self.delay+self.network.cells[c]["delay"]
                            self.clamps[-1].dur = max([self.network.cells[c]["mindur"], min([self.stim_dur,2])])
                        elif self.network.cells[c]["type"] == "Offset":
                            self.clamps[-1].delay = self.delay+self.stim_dur+self.network.cells[c]["delay"]
                            self.clamps[-1].dur = max([self.network.cells[c]["mindur"], min([self.stim_dur,2])])

                elif self.network.cells[c]["stim"] == "IClamp":
                    for i in self.network.cells[c]["cells"]:
                        self.clamps.append(neuron.h.IClamp(0.5, sec=i.sec["soma"]))
                        if self.network.cells[c]["type"] == "Sustained":
                            self.clamps.append(neuron.h.IClamp(0.5, sec=i.sec["soma"]))
                            self.clamps[-1].amp = self.network.cells[c]["stimamp"]
                            self.clamps[-1].dur = self.stim_dur
                            self.clamps[-1].delay = self.delay+self.network.cells[c]["delay"]
                        if self.network.cells[c]["type"] == "Onset":
                            self.clamps.append(neuron.h.IClamp(0.5, sec=i.sec["soma"]))
                            self.clamps[-1].amp = self.network.cells[c]["stimamp"]
                            self.clamps[-1].dur = 1
                            self.clamps[-1].delay = self.delay+self.network.cells[c]["delay"]
                        if self.network.cells[c]["type"] == "Offset":
                            self.clamps.append(neuron.h.IClamp(0.5, sec=i.sec["soma"]))
                            self.clamps[-1].amp = self.network.cells[c]["stimamp"]
                            self.clamps[-1].dur = 1
                            self.clamps[-1].delay = self.delay+self.stim_dur+self.network.cells[c]["delay"]

    def run(self):
        if self.verbose: print "Initializing Simulation"
        self.set_stim()
        neuron.h.dt = self.dt
        neuron.h.celsius = 36
        neuron.h.finitialize(-60)
        neuron.h.load_file('parcom.hoc')
        neuron.init()
        if self.verbose: print "...Running Simulation"
        neuron.run(self.sim_time)
        if self.verbose: print "...Simulation Complete\n"

