from sim import Simulation
import network
import neuron
import progress
from math import ceil


def run(netdef,modify,procs,thisProc,stims,param,repeats,sim_time,SaveSpikes,SaveVoltage):
    net = netdef()

    if SaveVoltage:
        net.recordVoltage()

    repeats = int(repeats)
    s = Simulation(net, randomseed=200)
    s.verbose = False
    s.sim_time = sim_time
    s.dt = 0.025
    total = len(stims)*len(param)*repeats
    spp = ceil(float(total)/procs)
    start = thisProc*spp
    end = (thisProc+1)*spp
    count = 0
    for a in param: 
        net = modify(net,a)
        for d in stims*repeats:
            progress.update(count-start, spp)
            if count >= start and count < end:
                s.stim_dur = d 
                s.run()
                key = [a,d] 
                net.savecells([["IC","soma"],["IC","dendE"]], key, spikes=SaveSpikes,voltage=SaveVoltage)
            count += 1
    progress.update(count-start, spp)

    r = [thisProc,net.savedparams,net.savedcells]
    return r


def C_SEARCH(net,a,getparams=False):
    if getparams:
        stims = [i for i in range(1,30)] + [i for i in range(30,251,10)]
        ondelay = [i for i in range(5,51,5)]
        offweight = [i*0.1 for i in range(0,21,2)]
        onweight = [i*0.1 for i in range(0,21,2)]
        param = []
        for a in ondelay:
            for b in offweight:
                for c in onweight:
                    param.append([a,b,c])
        return [stims, param]

    net.cells["MSO_ON"]["delay"] = a[0]
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(gmax=0.01*a[1], mg=0.5)
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyAMPA(gmax=0.005*a[1])
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=0.01*a[2], mg=0.5)
    net.cells["IC"]["cells"][0].sec["dendE"].modifyAMPA(gmax=0.005*a[2])
    return net


def AC_NMDA_BETA(net,a,getparams=False):
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(Beta=a, mg=0.5)
    return net

def C_NMDA_BETA(net,a,getparams=False):
    if getparams:
        stims = [i for i in range(1,25)] + [i for i in range(25,251,5)]
        param = [i*0.002 for i in range(1,22,1)]
        return [stims, param]
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(Beta=a, mg=0.5)
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(Beta=a, mg=0.5)
    return net
