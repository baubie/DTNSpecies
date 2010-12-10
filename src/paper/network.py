import cells
import neuron
import nrn
from copy import deepcopy

populationTemplate = {"stim": None, "delay": 0, "cells": [], "type": "Sustained", "mindur": 1}

class DTN_AntiCoincidence(object):
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




class DTN_Coincidence(object):
    def __init__(self):
        self.cells = {}
        self.cells["IC"] = deepcopy(populationTemplate)
        self.cells["DNLL"] = deepcopy(populationTemplate)
        self.cells["MSO_ON"] = deepcopy(populationTemplate)
        self.cells["MSO_OFF"] = deepcopy(populationTemplate)

        self.cells["IC"]["cells"].append(cells.IC_Neuron())

        # DNLL GABA input
        numDNLL = 50
        self.cells["DNLL"]["stim"] = "Poisson"
        self.cells["DNLL"]["delay"] = 0
        self.cells["DNLL"]["mindur"] = 2
        for i in range(numDNLL):
            self.cells["DNLL"]["cells"].append(cells.DNLL_Neuron())
        self.cells["IC"]["cells"][0].dendI.insertGABAa(numDNLL,0.00/numDNLL,1)
        for i in range(numDNLL):
            neuron.h.setpointer(self.cells["DNLL"]["cells"][i].soma(0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].dendI.GABAa[i])

        # MSO Glutamate input
        numMSO_ON = 100
        self.cells["MSO_ON"]["stim"] = "Poisson"
        self.cells["MSO_ON"]["type"] = "Onset"
        self.cells["MSO_ON"]["delay"] = 10
        for i in range(numMSO_ON):
            self.cells["MSO_ON"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].dendE.insertAMPA(numMSO_ON,0.0/numMSO_ON,1)
        for i in range(numMSO_ON):
            neuron.h.setpointer(self.cells["MSO_ON"]["cells"][i].soma(0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].dendE.AMPA[i])
        # MSO Glutamate input
        numMSO_OFF = 100
        self.cells["MSO_OFF"]["stim"] = "Poisson"
        self.cells["MSO_OFF"]["type"] = "Offset"
        self.cells["MSO_OFF"]["delay"] = 2
        for i in range(numMSO_OFF):
            self.cells["MSO_OFF"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].dendEOff.insertAMPA(numMSO_OFF,0.1/numMSO_OFF,1)
        for i in range(numMSO_OFF):
            neuron.h.setpointer(self.cells["MSO_OFF"]["cells"][i].soma(0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].dendEOff.AMPA[i])
