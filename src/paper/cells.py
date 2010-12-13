import models
import neuron

class Neuron:
    def __init__(self):
        self.sec = {}

class DNLL_Neuron(Neuron):
    def __init__(self):
        Neuron.__init__(self)
        self.sec["soma"] = models.DNLL_Soma()
        self.getStim = False

class MSO_Neuron(Neuron):
    def __init__(self):
        Neuron.__init__(self)
        self.sec["soma"] = models.MSO_Soma()
        self.getStim = False

class IC_Neuron(Neuron):
    def __init__(self):
        Neuron.__init__(self)
        self.sec["soma"] = models.IC_Soma()
        self.sec["dendI"] = models.IC_Dendrite()
        self.sec["dendE"] = models.IC_Dendrite()
        self.sec["dendEOff"] = models.IC_Dendrite()

        self.sec["dendE"].L = 100
        self.sec["dendEOff"].L = 100
        self.sec["dendI"].L = 100

        self.sec["dendI"].nseg = 19
        self.sec["dendE"].nseg = 19
        self.sec["dendEOff"].nseg = 19

        self.sec["dendI"].connect(self.sec["soma"])
        self.sec["dendE"].connect(self.sec["soma"])
        self.sec["dendEOff"].connect(self.sec["soma"])
        self.getStim = False

