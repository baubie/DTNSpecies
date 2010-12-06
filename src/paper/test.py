import neuron
import nrn


class Simulation:

    def __init__(self):
        self.soma = nrn.Section()
        self.IC = []

    def set_stim(self):
        self.r = neuron.h.Random(100)
        self.r.poisson(0.05)
        self.IC.append(neuron.h.IClamp(0.5, sec=self.soma))
        self.IC[-1].dur = 10
        self.IC[-1].delay = 10
        self.r.play(self.IC[-1]._ref_amp)


    def run(self):
        self.set_stim()
        neuron.h.finitialize(-65)
        neuron.init()
        neuron.run(100)

s = Simulation()
s.run()
s.run()
