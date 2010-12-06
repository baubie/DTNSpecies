from neuron import h, nrn
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

        # Receptor Lists
        self.GABAa = []
        self.GABAb = []
        self.AMPA = []
        self.NMDA = []

    def show(self):
        x = np.array(self.rec_t)
        y = np.array(self.rec_v)
        plt.plot(x,y)
        plt.xlabel("Time (ms)")
        plt.ylabel("Voltage (mV")
        plt.axis(ymin=-90, ymax=50)
        
    def insertGABAa(self, num=1, gmax=0.015, pos=1):
        for i in range(num):
            self.GABAa.append(h.GABAa(self(pos)))
            self.GABAa[-1].gmax = gmax

    def modifyGABAa(self, Cmax=1, Cdur=1, Alpha=5, Beta=0.18, Erev=-80, Prethresh=0, Deadtime=1):
        for i in range(len(self.GABAa)):
            self.GABAa[i].Cmax = Cmax
            self.GABAa[i].Cdur = Cdur
            self.GABAa[i].Alpha = Alpha 
            self.GABAa[i].Beta = Beta
            self.GABAa[i].Erev = Erev
            self.GABAa[i].Prethresh = Prethresh
            self.GABAa[i].Deadtime = Deadtime



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
        self.g = 1.0/2000.0 # tau = 2 ms
        self(0.25).pas.e = self.E
        self(0.25).pas.g = self.g
        self(0.50).pas.e = self.E
        self(0.50).pas.g = self.g
        self(0.75).pas.e = self.E
        self(0.75).pas.g = self.g



class IC_Soma(ModelBase):
    def __init__(self):
        ModelBase.__init__(self)
        self.setup()

    def setup(self):
        self.nseg = 1
        self.insert('hh2')
        self.insert('pas')

        self.E = -55
        self(0.5).pas.g = 1.0/2000.0 # tau = 2 ms
        self(0.5).pas.e = self.E
        self(0.5).ena = 50
        self(0.5).ek = -90
        self(0.5).hh2.gnabar = 0.030
        self(0.5).hh2.gkbar = 0.005
        self(0.5).hh2.vtraub = -55

        self.L = 15
        self.diam = 15


