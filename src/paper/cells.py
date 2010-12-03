import models
import neuron

class DNLL_Neuron:
    def __init__(self):
        self.soma = models.DNLL_Soma()
        self.getStim = False

class IC_Neuron:
    def __init__(self):
        self.soma = models.IC_Soma()
        self.dend = models.IC_Dendrite()
        self.dend.connect(self.soma)
        self.getStim = False

