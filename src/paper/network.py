import cells
import neuron
import nrn
from copy import deepcopy

populationTemplate = {"stim": None, "delay": 0, "cells": [], "type": "Sustained"}

class DTN(object):

    def __init__(self):
        self.cells = {}
        self.cells["IC"] = deepcopy(populationTemplate)
        self.cells["DNLL"] = deepcopy(populationTemplate)
        self.cells["MSO"] = deepcopy(populationTemplate)

        self.cells["IC"]["cells"].append(cells.IC_Neuron())

        # DNLL GABA input
        numDNLL = 50
        self.cells["DNLL"]["stim"] = "Poisson"
        self.cells["DNLL"]["delay"] = 0
        for i in range(numDNLL):
            self.cells["DNLL"]["cells"].append(cells.DNLL_Neuron())
        self.cells["IC"]["cells"][0].dendI.insertGABAa(numDNLL,0.02/numDNLL,1)
        for i in range(numDNLL):
            neuron.h.setpointer(self.cells["DNLL"]["cells"][i].soma(0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].dendI.GABAa[i])

        # MSO Glutamate input
        numMSO = 100
        self.cells["MSO"]["stim"] = "Poisson"
        self.cells["MSO"]["type"] = "Onset"
        self.cells["MSO"]["delay"] = 5
        for i in range(numMSO):
            self.cells["MSO"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].dendE.insertAMPA(numMSO,0.02/numMSO,1)
        for i in range(numMSO):
            neuron.h.setpointer(self.cells["MSO"]["cells"][i].soma(0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].dendE.AMPA[i])

