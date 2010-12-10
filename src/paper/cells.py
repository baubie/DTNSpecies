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
        self.dendEOff = models.IC_Dendrite()

        self.dendE.L = 200
        self.dendEOff.L = 200
        self.dendI.L = 200

        self.dendI.nseg = 19
        self.dendE.nseg = 19
        self.dendEOff.nseg = 19

        self.dendI.connect(self.soma)
        self.dendE.connect(self.soma)
        self.dendEOff.connect(self.soma)
        self.getStim = False

