from neuron import h
import nrn
import matplotlib.pyplot as plt
import numpy as np

class ModelBase(nrn.Section):
    def __init__(self):
        nrn.Section.__init__(self)

        # Provide recording variables
        self.rec_v = h.Vector()
        self.rec_v.record(self(0.5)._ref_v)

        self.rec_t = h.Vector()
        self.rec_t.record(h._ref_t)

    def show(self):
        x = np.array(self.rec_t)
        y = np.array(self.rec_v)
        plt.plot(x,y)
        plt.xlabel("Time (ms)")
        plt.ylabel("Voltage (mV")
        plt.axis(ymin=-90, ymax=50)


# DNLL Mostly projects GABA to the IC
class DNLL_Soma(ModelBase):
    def __init__(self):
        ModelBase.__init__(self)
        self.setup()

    def setup(self):
        self.nseg = 1
        self.insert('hh2')
        self.insert('pas')

        self.E = -55
        self(0.5).pas.g = 1.0/5000.0 # tau = 5 ms
        self(0.5).pas.e = self.E
        self(0.5).ena = 50
        self(0.5).ek = -90
        self(0.5).hh2.gnabar = 0.030
        self(0.5).hh2.gkbar = 0.005
        self(0.5).hh2.vtraub = -55

        self.L = 15
        self.diam = 15


class IC_Dendrite(ModelBase):
    def __init__(self):
        ModelBase.__init__(self)
        self.setup()

    def setup(self):
        self.nseg = 3
        self.insert('pas')
        self.L = 300
        self.diam = 2

        self.E = -55
        self.g = 1.0/2000.0 # tau = 5 ms
        self(0.25).pas.e = self.E
        self(0.25).pas.g = self.g
        self(0.50).pas.e = self.E
        self(0.50).pas.g = self.g
        self(0.75).pas.e = self.E
        self(0.75).pas.g = self.g

        self.GABAa = h.GABAa(self(1))
        self.GABAa.gmax = 0.015
        dummy = nrn.Section()
        h.setpointer(dummy(0.5)._ref_v, 'pre', self.GABAa)



class IC_Soma(ModelBase):
    def __init__(self):
        ModelBase.__init__(self)
        self.setup()

    def setup(self):
        self.nseg = 1
        self.insert('hh2')
        self.insert('pas')

        self.E = -55
        self(0.5).pas.g = 1.0/2000.0 # tau = 5 ms
        self(0.5).pas.e = self.E
        self(0.5).ena = 50
        self(0.5).ek = -90
        self(0.5).hh2.gnabar = 0.030
        self(0.5).hh2.gkbar = 0.005
        self(0.5).hh2.vtraub = -55

        self.GABAa = h.GABAa(self(1))
        self.GABAa.gmax = 0.015
        dummy = nrn.Section()
        h.setpointer(dummy(0.5)._ref_v, 'pre', self.GABAa)

        self.L = 15
        self.diam = 15

