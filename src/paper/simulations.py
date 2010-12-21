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


def AC_NMDA_BETA(net,a):
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(Beta=a, mg=0.5)
    return net

def C_NMDA_BETA(net,a):
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(Beta=a, mg=0.5)
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(Beta=a, mg=0.5)
    return net
