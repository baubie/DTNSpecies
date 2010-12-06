import cells
import neuron
import nrn
from copy import deepcopy

populationTemplate = {"getStim": False, "cells": []}

class DTN(object):

    def __init__(self):
        self.cells = {}
        self.cells["IC"] = deepcopy(populationTemplate)
        self.cells["DNLL"] = deepcopy(populationTemplate)

        self.cells["IC"]["cells"].append(cells.IC_Neuron())

        numDNLL = 2
        self.cells["DNLL"]["getStim"] = True
        for i in range(numDNLL):
            self.cells["DNLL"]["cells"].append(cells.DNLL_Neuron())

        self.cells["IC"]["cells"][0].dend.insertGABAa(numDNLL,0.015,1)
        for i in range(numDNLL):
            neuron.h.setpointer(self.cells["DNLL"]["cells"][i].soma(0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].dend.GABAa[i])
