import cells
import neuron
import nrn
from copy import deepcopy

populationTemplate = {"stim": None, "delay": 0, "cells": [], "type": "Sustained", "mindur": 1}

class network(object):
    def __init__(self):
        self.cells = {}
        self.savedcells = []
        self.savedparams = []

    def savecells(self, names, param, spikes=True, voltage=False):
        saved = {}
        self.savedparams.append(param)
        for n in names:
            population = []
            for c in self.cells[n[0]]["cells"]:
                cell = {"rec_v": [], "rec_t": [], "rec_s": []}

                if spikes:
                    cell["rec_s"] = list(c.sec[n[1]].rec_s)

                if voltage:
                    cell["rec_v"] = list(c.sec[n[1]].rec_v)
                    cell["rec_t"] = list(c.sec[n[1]].rec_t)

                population.append(cell)
            saved[n[0]+"-"+n[1]] = population 
        self.savedcells.append(saved)


class DTN_AntiCoincidence(network):
    def __init__(self):
        network.__init__(self)
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
        self.cells["IC"]["cells"][0].sec["dendI"].insertGABAa(numDNLL,0.02/numDNLL,1)
        for i in range(numDNLL):
            neuron.h.setpointer(self.cells["DNLL"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendI"].GABAa[i])

        # MSO Glutamate input
        numMSO = 100
        self.cells["MSO"]["stim"] = "Poisson"
        self.cells["MSO"]["type"] = "Onset"
        self.cells["MSO"]["delay"] = 5
        for i in range(numMSO):
            self.cells["MSO"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].sec["dendE"].insertAMPA(numMSO,0.02/numMSO,1)
        self.cells["IC"]["cells"][0].sec["dendE"].insertNMDA(numMSO,0.02/numMSO,1)
        for i in range(numMSO):
            neuron.h.setpointer(self.cells["MSO"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendE"].AMPA[i])
            neuron.h.setpointer(self.cells["MSO"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendE"].NMDA[i])



class DTN_Coincidence(network):
    def __init__(self):
        network.__init__(self)
        self.cells["IC"] = deepcopy(populationTemplate)
        self.cells["DNLL"] = deepcopy(populationTemplate)
        self.cells["MSO_ON"] = deepcopy(populationTemplate)
        self.cells["MSO_OFF"] = deepcopy(populationTemplate)

        numIC = 2
        self.cells["IC"]["cells"].append(cells.IC_Neuron())

        # DNLL GABA input
        numDNLL = 50
        self.cells["DNLL"]["stim"] = "Poisson"
        self.cells["DNLL"]["delay"] = 0
        self.cells["DNLL"]["mindur"] = 2
        for i in range(numDNLL):
            self.cells["DNLL"]["cells"].append(cells.DNLL_Neuron())
        self.cells["IC"]["cells"][0].sec["dendI"].insertGABAa(numDNLL,0.02/numDNLL,1)
        for i in range(numDNLL):
            neuron.h.setpointer(self.cells["DNLL"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendI"].GABAa[i])

        # MSO Glutamate input
        numMSO_ON = 100
        self.cells["MSO_ON"]["stim"] = "Poisson"
        self.cells["MSO_ON"]["type"] = "Onset"
        self.cells["MSO_ON"]["delay"] = 8
        for i in range(numMSO_ON):
            self.cells["MSO_ON"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].sec["dendE"].insertAMPA(numMSO_ON,0.005/numMSO_ON,1)
        self.cells["IC"]["cells"][0].sec["dendE"].insertNMDA(numMSO_ON,0.005/numMSO_ON,1)

        for i in range(numMSO_ON):
            neuron.h.setpointer(self.cells["MSO_ON"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendE"].AMPA[i])
            neuron.h.setpointer(self.cells["MSO_ON"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendE"].NMDA[i])

        # MSO Glutamate input
        numMSO_OFF = 100
        self.cells["MSO_OFF"]["stim"] = "Poisson"
        self.cells["MSO_OFF"]["type"] = "Offset"
        self.cells["MSO_OFF"]["delay"] = 2
        for i in range(numMSO_OFF):
            self.cells["MSO_OFF"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].sec["dendEOff"].insertAMPA(numMSO_OFF,0.01/numMSO_OFF,1)
        self.cells["IC"]["cells"][0].sec["dendEOff"].insertNMDA(numMSO_OFF,0.01/numMSO_OFF,1)
        for i in range(numMSO_OFF):
            neuron.h.setpointer(self.cells["MSO_OFF"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendEOff"].AMPA[i])
            neuron.h.setpointer(self.cells["MSO_OFF"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendEOff"].NMDA[i])



