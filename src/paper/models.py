from neuron import hclass, h
import nrn


class IC_Dendrite(nrn.Section):
    def __init__(self):
        nrn.Section.__init__(self)

        # Provide recording variables
        self.rec_v = h.Vector()
        self.rec_v.record(self(0.5)._ref_v)

        self.setup()

    def setup(self):
        self.nseg = 3
        self.insert('pas')

        self.E = -55
        self(0.5).pas.g = 1/5000 # tau = 5 ms
        self(0.5).pas.e = self.E
        self.L = 300
        self.diam = 3


class IC_Soma(nrn.Section):
    def __init__(self):
        nrn.Section.__init__(self)

        # Provide recording variables
        self.rec_v = h.Vector()
        self.rec_v.record(self(0.5)._ref_v)

        self.setup()

    def setup(self):
        self.nseg = 1
        self.insert('hh2')
        self.insert('pas')

        self.E = -55
        self(0.5).pas.g = 1/5000 # tau = 5 ms
        self(0.5).pas.e = self.E
        self(0.5).ena = 50
        self(0.5).ek = -90
        self(0.5).hh2.gnabar = 0.030
        self(0.5).hh2.gkbar = 0.005
        self(0.5).hh2.vtraub = -55

        self.L = 15
        self.diam = 15

