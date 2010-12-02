import models

class IC_Neuron:
    def __init__(self):
        self.soma = models.IC_Soma()
        self.dend = models.IC_Dendrite()
        self.dend.connect(self.soma)
