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

        self.time_sec = nrn.Section()
        self.rec_t = neuron.h.Vector()
        self.rec_t.record(neuron.h._ref_t, sec=self.time_sec)

        self.sim_amp = 0.05

    def recordVoltage(self):
        for pops in self.cells:
            for c in self.cells[pops]["cells"]:
                for s in c.sec:
                    c.sec[s].recordVoltage()


    def savecells(self, names, param, spikes=True, conductance=False, current=False, voltage=False):
        saved = {}
        self.savedparams.append(param)
        for n in names:
            population = []
            for c in self.cells[n[0]]["cells"]:
                cell = {"rec_v": [], "rec_t": [], "rec_s": [], "rec_AMPAg": [], "rec_NMDAg": [], "rec_GABAag": [], "rec_AMPAi": [], "rec_NMDAi": [], "rec_GABAai": []}
                if current:
                    cell["rec_NMDAi"] = list(c.sec[n[1]].NMDAi())
                    cell["rec_AMPAi"] = list(c.sec[n[1]].AMPAi())
                    cell["rec_GABAai"] = list(c.sec[n[1]].GABAai())
                    cell["rec_t"] = list(self.rec_t)

                if conductance:
                    cell["rec_NMDAg"] = list(c.sec[n[1]].NMDAg())
                    cell["rec_AMPAg"] = list(c.sec[n[1]].AMPAg())
                    cell["rec_GABAag"] = list(c.sec[n[1]].GABAag())
                    cell["rec_t"] = list(self.rec_t)

                if spikes:
                    cell["rec_s"] = list(c.sec[n[1]].rec_s)

                if voltage:
                    cell["rec_v"] = list(c.sec[n[1]].rec_v)
                    cell["rec_t"] = list(self.rec_t)

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
        self.cells["IC"]["cells"][0].sec["dendI"].insertGABAa(num=numDNLL,gmax=0.02/numDNLL,pos=1)
        for i in range(numDNLL):
            neuron.h.setpointer(self.cells["DNLL"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendI"].GABAa[i])

        # MSO Glutamate input
        numMSO = 100
        self.cells["MSO"]["stim"] = "Poisson"
        self.cells["MSO"]["type"] = "Onset"
        self.cells["MSO"]["delay"] = 2
        self.cells["MSO"]["mindur"] = 2
        for i in range(numMSO):
            self.cells["MSO"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].sec["dendE"].insertAMPA(num=numMSO,gmax=0.03/numMSO,pos=1)
        self.cells["IC"]["cells"][0].sec["dendE"].insertNMDA(num=numMSO,gmax=0.05/numMSO,pos=1)
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

        numIC = 1
        self.cells["IC"]["cells"].append(cells.IC_Neuron())

        # DNLL GABA input
        numDNLL = 5
        self.cells["DNLL"]["stim"] = "Poisson"
        self.cells["DNLL"]["delay"] = 0
        self.cells["DNLL"]["mindur"] = 1
        for i in range(numDNLL):
            self.cells["DNLL"]["cells"].append(cells.DNLL_Neuron())
        self.cells["IC"]["cells"][0].sec["dendI"].insertGABAa(num=numDNLL,gmax=0.002/numDNLL,pos=1)
        for i in range(numDNLL):
            neuron.h.setpointer(self.cells["DNLL"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendI"].GABAa[i])

        # MSO Glutamate input
        numMSO_ON = 1
        self.cells["MSO_ON"]["stim"] = "Poisson"
        self.cells["MSO_ON"]["type"] = "Onset"
        self.cells["MSO_ON"]["delay"] = 15
        for i in range(numMSO_ON):
            self.cells["MSO_ON"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].sec["dendE"].insertAMPA(num=numMSO_ON,gmax=0.005/numMSO_ON,pos=1)
        self.cells["IC"]["cells"][0].sec["dendE"].insertNMDA(num=numMSO_ON,gmax=0.01/numMSO_ON,pos=1)

        for i in range(numMSO_ON):
            neuron.h.setpointer(self.cells["MSO_ON"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendE"].AMPA[i])
            neuron.h.setpointer(self.cells["MSO_ON"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendE"].NMDA[i])

        # MSO Glutamate input
        numMSO_OFF = 1
        self.cells["MSO_OFF"]["stim"] = "Poisson"
        self.cells["MSO_OFF"]["type"] = "Offset"
        self.cells["MSO_OFF"]["delay"] = 1
        for i in range(numMSO_OFF):
            self.cells["MSO_OFF"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].sec["dendEOff"].insertAMPA(num=numMSO_OFF,gmax=0.005/numMSO_OFF,pos=1)
        self.cells["IC"]["cells"][0].sec["dendEOff"].insertNMDA(num=numMSO_OFF,gmax=0.01/numMSO_OFF,pos=1)
        for i in range(numMSO_OFF):
            neuron.h.setpointer(self.cells["MSO_OFF"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendEOff"].AMPA[i])
            neuron.h.setpointer(self.cells["MSO_OFF"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["dendEOff"].NMDA[i])




class DTN_CoincidenceSimple(network):
    def __init__(self):
        network.__init__(self)
        self.cells["IC"] = deepcopy(populationTemplate)
        self.cells["DNLL"] = deepcopy(populationTemplate)
        self.cells["MSO_ON"] = deepcopy(populationTemplate)
        self.cells["MSO_OFF"] = deepcopy(populationTemplate)

        con = False

        numIC = 1
        self.cells["IC"]["cells"].append(cells.IC_NeuronSoma())

        # DNLL GABA input
        numDNLL = 10
        self.cells["DNLL"]["stim"] = "Poisson"
        self.cells["DNLL"]["delay"] = 0
        self.cells["DNLL"]["mindur"] = 1
        for i in range(numDNLL):
            self.cells["DNLL"]["cells"].append(cells.DNLL_Neuron())
        self.cells["IC"]["cells"][0].sec["soma"].insertGABAa(recConductance=con,num=numDNLL,gmax=0.0035/numDNLL,pos=0.5)
        for i in range(numDNLL):
            neuron.h.setpointer(self.cells["DNLL"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["soma"].GABAa[i])

        # MSO Glutamate input
        numMSO_ON = 1
        self.cells["MSO_ON"]["stim"] = "Poisson"
        self.cells["MSO_ON"]["type"] = "Onset"
        self.cells["MSO_ON"]["delay"] = 15
        for i in range(numMSO_ON):
            self.cells["MSO_ON"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].sec["soma"].insertAMPA(recConductance=con,num=numMSO_ON,gmax=0.003/numMSO_ON,pos=0.5)
        self.cells["IC"]["cells"][0].sec["soma"].insertNMDA(recConductance=con,num=numMSO_ON,gmax=0.005/numMSO_ON,pos=0.5)

        for i in range(numMSO_ON):
            neuron.h.setpointer(self.cells["MSO_ON"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["soma"].AMPA[i])
            neuron.h.setpointer(self.cells["MSO_ON"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["soma"].NMDA[i])

        # MSO Glutamate input
        numMSO_OFF = 1
        self.cells["MSO_OFF"]["stim"] = "Poisson"
        self.cells["MSO_OFF"]["type"] = "Offset"
        self.cells["MSO_OFF"]["delay"] = 1
        for i in range(numMSO_OFF):
            self.cells["MSO_OFF"]["cells"].append(cells.MSO_Neuron())
        self.cells["IC"]["cells"][0].sec["soma"].insertAMPA(recConductance=con,num=numMSO_OFF,gmax=0.003/numMSO_OFF,pos=0.5)
        self.cells["IC"]["cells"][0].sec["soma"].insertNMDA(recConductance=con,num=numMSO_OFF,gmax=0.005/numMSO_OFF,pos=0.5)
        for i in range(numMSO_OFF):
            neuron.h.setpointer(self.cells["MSO_OFF"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["soma"].AMPA[i+numMSO_ON])
            neuron.h.setpointer(self.cells["MSO_OFF"]["cells"][i].sec["soma"](0.5)._ref_v, 'pre', self.cells["IC"]["cells"][0].sec["soma"].NMDA[i+numMSO_ON])



