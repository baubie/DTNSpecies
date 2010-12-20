from sim import Simulation
import network
import neuron
import progress
from math import ceil

def sims_per_proc(procs, total):
    return ceil(float(total)/procs)

def NMDA_BETA_AC(AvailProcs,ThisProc,stims,param,repeats,ShowSpikes=False, ShowVoltage=False):
    repeats = int(repeats)
    net = network.DTN_AntiCoincidence()
    s = Simulation(net, randomseed=200)
    s.verbose = False
    s.sim_time = 200
    s.dt = 0.025
    count = 0
    total = len(stims)*len(param)*repeats
    spp = sims_per_proc(AvailProcs, total)
    start = ThisProc*spp
    end = (ThisProc+1)*spp
    for a in param: 
        net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(Beta=a, mg=0.5)
        for d in stims*repeats:
            progress.update(count-start, spp)
            if count >= start and count < end:
                s.stim_dur = d 
                s.run()
                key = [a,d] 
                net.savecells([["IC","soma"],["IC","dendE"]], key, spikes=ShowSpikes,conductance=False,current=False,voltage=ShowVoltage)
            count += 1

    return [ThisProc,net.savedparams,net.savedcells]

