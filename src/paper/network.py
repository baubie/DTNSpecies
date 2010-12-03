import cells
import neuron
import nrn

class DTN(object):

    def __init__(self):
        self.cells = {}
        self.cells["IC"] = cells.IC_Neuron()
        self.cells["DNLL"] = cells.DNLL_Neuron()
        self.cells["DNLL"].getStim = True


        neuron.h.setpointer(self.cells["DNLL"].soma(0.5)._ref_v, 'pre', self.cells["IC"].dend.GABAa)
        self.cells["IC"].dend.GABAa.gmax = 0.1
