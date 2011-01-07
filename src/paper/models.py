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

        self.nc = h.NetCon(self(0.5)._ref_v, None, sec=self)
        self.rec_s = h.Vector()
        self.nc.record(self.rec_s)

        self.rec_NMDA_i = []
        self.rec_AMPA_i = []
        self.rec_GABAa_i = []

        self.rec_NMDA_g = []
        self.rec_AMPA_g = []
        self.rec_GABAa_g = []
        
        # Receptor Lists
        self.GABAa = []
        self.GABAb = []
        self.AMPA = []
        self.NMDA = []

    def recordVoltage(self):
        self.rec_v.record(self(0.5)._ref_v)

    def NMDAi(self):
        if len(self.NMDA) == 0: return []
        r = [0]*len(self.rec_NMDA_i[0])
        for i in self.rec_NMDA_i:
            for t in range(len(i)):
                r[t] += i[t]
        r = [i/float(len(self.NMDA)) for i in r]
        return r

    def AMPAi(self):
        if len(self.AMPA) == 0: return []
        r = [0]*len(self.rec_AMPA_i[0])
        for i in self.rec_AMPA_i:
            for t in range(len(i)):
                r[t] += i[t]
        r = [i/len(self.AMPA) for i in r]
        return r

    def GABAai(self):
        if len(self.GABAa) == 0: return []
        r = [0]*len(self.rec_GABAa_i[0])
        for i in self.rec_GABAa_i:
            for t in range(len(i)):
                r[t] += i[t]
        r = [i/len(self.GABAa) for i in r]
        return r

    def NMDAg(self):
        if len(self.NMDA) == 0: return []
        r = [0]*len(self.rec_NMDA_g[0])
        for i in self.rec_NMDA_g:
            for t in range(len(i)):
                r[t] += i[t]
        r = [i/float(len(self.NMDA)) for i in r]
        return r

    def AMPAg(self):
        if len(self.AMPA) == 0: return []
        r = [0]*len(self.rec_AMPA_g[0])
        for i in self.rec_AMPA_g:
            for t in range(len(i)):
                r[t] += i[t]
        r = [i/len(self.AMPA) for i in r]
        return r

    def GABAag(self):
        if len(self.GABAa) == 0: return []
        r = [0]*len(self.rec_GABAa_g[0])
        for i in self.rec_GABAa_g:
            for t in range(len(i)):
                r[t] += i[t]
        r = [i/len(self.GABAa) for i in r]
        return r

    def insertGABAa(self, recConductance=False, recCurrent=False, num=1, gmax=0.015, pos=1):
        for i in range(num):
            self.GABAa.append(h.GABAa(self(pos)))
            self.GABAa[-1].gmax = gmax
            if recCurrent:
                self.rec_GABAa_i.append(h.Vector())
                self.rec_GABAa_i[-1].record(self.GABAa[-1]._ref_i)
            if recConductance:
                self.rec_GABAa_g.append(h.Vector())
                self.rec_GABAa_g[-1].record(self.GABAa[-1]._ref_g)

    def insertAMPA(self, recConductance=False, recCurrent=False,  num=1, gmax=0.015, pos=1):
        for i in range(num):
            self.AMPA.append(h.AMPA(self(pos)))
            self.AMPA[-1].gmax = gmax
            if recCurrent:
                self.rec_AMPA_i.append(h.Vector())
                self.rec_AMPA_i[-1].record(self.AMPA[-1]._ref_i)
            if recConductance:
                self.rec_AMPA_g.append(h.Vector())
                self.rec_AMPA_g[-1].record(self.AMPA[-1]._ref_g)

    def insertNMDA(self, recConductance=False, recCurrent=False,  num=1, gmax=0.015, pos=1):
        for i in range(num):
            self.NMDA.append(h.NMDA(self(pos)))
            self.NMDA[-1].gmax = gmax
            if recCurrent:
                self.rec_NMDA_i.append(h.Vector())
                self.rec_NMDA_i[-1].record(self.NMDA[-1]._ref_i)
            if recConductance:
                self.rec_NMDA_g.append(h.Vector())
                self.rec_NMDA_g[-1].record(self.NMDA[-1]._ref_g)

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
        self(0.5).hh2.gnabar = 0.100
        self(0.5).hh2.gkbar = 0.005
        self(0.5).hh2.vtraub = -40

        self.L = 13
        self.diam = 13
