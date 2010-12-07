import models
import neuron

class DNLL_Neuron:
    def __init__(self):
        self.soma = models.DNLL_Soma()
        self.getStim = False

class MSO_Neuron:
    def __init__(self):
        self.soma = models.MSO_Soma()
        self.getStim = False

class IC_Neuron:
    def __init__(self):
        self.soma = models.IC_Soma()
        self.dendI = models.IC_Dendrite()
        self.dendE = models.IC_Dendrite()
        self.dendE.L = 50
        self.dendI.L = 50
        self.dendI.connect(self.soma)
        self.dendE.connect(self.soma)
        self.getStim = False

