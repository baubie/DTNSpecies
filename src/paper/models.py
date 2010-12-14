from neuron import h, nrn
import numpy as np


def nstomhocm(ns,area):
    # Convert nS to mho/cm^2
    # This allows us to decide on a total conductance
    # independent of soma size
    return 1E-9*ns/area



class ModelBase(nrn.Section):
    def __init__(self):
        nrn.Section.__init__(self)

        # Provide recording variables
        self.rec_v = h.Vector()
        self.rec_v.record(self(0.5)._ref_v)

        self.rec_t = h.Vector()
        self.rec_t.record(h._ref_t)

        self.nc = h.NetCon(self(0.5)._ref_v, None, sec=self)
        self.rec_s = h.Vector()
        self.nc.record(self.rec_s)
        

        # Receptor Lists
        self.GABAa = []
        self.GABAb = []
        self.AMPA = []
        self.NMDA = []

    def insertGABAa(self, num=1, gmax=0.015, pos=1):
        for i in range(num):
            self.GABAa.append(h.GABAa(self(pos)))
            self.GABAa[-1].gmax = gmax

    def insertAMPA(self, num=1, gmax=0.015, pos=1):
        for i in range(num):
            self.AMPA.append(h.AMPA(self(pos)))
            self.AMPA[-1].gmax = gmax

    def insertNMDA(self, num=1, gmax=0.015, pos=1):
        for i in range(num):
            self.NMDA.append(h.NMDA(self(pos)))
            self.NMDA[-1].gmax = gmax

    def modifyGABAa(self, gmax=None, Cmax=None, Cdur=None, Alpha=None, Beta=None, Erev=None, Prethresh=None, Deadtime=None):
        for i in range(len(self.GABAa)):
            if gmax != None: self.GABAa[i].gmax = gmax/float(len(self.GABAa))
            if Cmax != None: self.GABAa[i].Cmax = Cmax
            if Cdur != None: self.GABAa[i].Cdur = Cdur
            if Alpha != None: self.GABAa[i].Alpha = Alpha 
            if Beta != None: self.GABAa[i].Beta = Beta
            if Erev != None: self.GABAa[i].Erev = Erev
            if Prethresh != None: self.GABAa[i].Prethresh = Prethresh
            if Deadtime != None: self.GABAa[i].Deadtime = Deadtime

    def modifyAMPA(self, gmax=None, Cmax=None, Cdur=None, Alpha=None, Beta=None, Erev=None, Prethresh=None, Deadtime=None):
        for i in range(len(self.AMPA)):
            if gmax != None: self.AMPA[i].gmax = gmax/float(len(self.AMPA))
            if Cmax != None: self.AMPA[i].Cmax = Cmax
            if Cdur != None: self.AMPA[i].Cdur = Cdur
            if Alpha != None: self.AMPA[i].Alpha = Alpha 
            if Beta != None: self.AMPA[i].Beta = Beta
            if Erev != None: self.AMPA[i].Erev = Erev
            if Prethresh != None: self.AMPA[i].Prethresh = Prethresh
            if Deadtime != None: self.AMPA[i].Deadtime = Deadtime

    def modifyNMDA(self, gmax=None, mg=None, Cmax=None, Cdur=None, Alpha=None, Beta=None, Erev=None, Prethresh=None, Deadtime=None):
        for i in range(len(self.NMDA)):
            if gmax != None: self.NMDA[i].gmax = gmax/float(len(self.NMDA))
            if Cmax != None: self.NMDA[i].Cmax = Cmax
            if Cdur != None: self.NMDA[i].Cdur = Cdur
            if Alpha != None: self.NMDA[i].Alpha = Alpha 
            if Beta != None: self.NMDA[i].Beta = Beta
            if Erev != None: self.NMDA[i].Erev = Erev
            if Prethresh != None: self.NMDA[i].Prethresh = Prethresh
            if Deadtime != None: self.NMDA[i].Deadtime = Deadtime
            if mg != None: self.NMDA[i].mg = mg


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
        self(0.5).pas.g = 1.0/1000.0 # tau = 5 ms
        self(0.5).pas.e = self.E
        self(0.5).ena = 50
        self(0.5).ek = -90
        self(0.5).hh2.gnabar = 0.1
        self(0.5).hh2.gkbar = 0.03
        self(0.5).hh2.vtraub = -54

        self.L = 10
        self.diam = 10

# MSO Onset cell
class MSO_Soma(ModelBase):
    def __init__(self):
        ModelBase.__init__(self)
        self.setup()

    def setup(self):
        self.nseg = 1
        self.insert('hh2')
        self.insert('pas')

        #gkltbar = 0.01592 (mho/cm2) <0,1e9>
        self.E = -55
        self(0.5).pas.g = 1.0/1000.0 # tau = 5 ms
        self(0.5).pas.e = self.E
        self(0.5).ena = 50
        self(0.5).ek = -90
        self(0.5).hh2.gnabar = 0.1
        self(0.5).hh2.gkbar = 0.03
        self(0.5).hh2.vtraub = -57

        self.L = 10
        self.diam = 10


class IC_Dendrite(ModelBase):
    def __init__(self):
        ModelBase.__init__(self)
        self.rec_v = h.Vector()
        self.rec_v.record(self(0.9)._ref_v)
        self.setup()

    def setup(self):
        self.nseg = 9
        self.insert('pas')
        self.L = 300
        self.diam = 1.5
        self.Ra = 35.4

        self.E = -55
        self.g = 1.0/10000.0 # tau = 10 ms
        for seg in self:
            seg.pas.e = self.E
            seg.pas.g = self.g


class IC_Soma(ModelBase):
    def __init__(self):
        ModelBase.__init__(self)
        self.setup()

    def setup(self):
        self.nseg = 1
        self.insert('hh2')
        self.insert('pas')

        self.cm=1.0
        self.Ra = 35.4
        self.E = -65
        self(0.5).pas.g = 1.0/5000.0 # tau = 2 ms
        self(0.5).pas.e = self.E
        self(0.5).ena = 50
        self(0.5).ek = -90
        self(0.5).hh2.gnabar = 0.10
        self(0.5).hh2.gkbar = 0.03
        self(0.5).hh2.vtraub = -52

        self.L = 13
        self.diam = 13
