from sim import Simulation
import network
import neuron
import progress
from math import ceil


def run(netdef,tosave,modify,procs,thisProc,stims,param,repeats,sim_time,SaveSpikes,SaveVoltage):
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
        s.set_amplitude(net.sim_amp)
        for d in stims*repeats:
            if count >= start and count < end:
                net = modify(net,a,d)
                progress.update(count-start,spp,thisProc)
                s.stim_dur = d 
                s.run()
                key = [a,d] 
                net.savecells(tosave, key, spikes=SaveSpikes,voltage=SaveVoltage)
            count += 1
    progress.update(spp,spp,thisProc)

    r = [thisProc,net.savedparams,net.savedcells]
    return r


def C_NMDA_BETA_SIMPLE(net,a,stim,getparams=False):
    if getparams:
        stims = [1,3,10,15,25]
        stims = range(1,26,1)
        param = [0.0066]
        return [1,100,stims, param]

    if stim <= 1:
        mult = 0.5*(stim)
    else:
        mult = 1.0

    net.cells["IC"]["cells"][0].sec["dendI"].modifyGABAa(gmax=0.015, Beta=0.18)
    net.cells["IC"]["cells"][0].sec["dendE"].modifyAMPA(gmax=0.010*mult)
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=0.010, Beta=a, mg=1.0)

    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyAMPA(gmax=0.010*mult)
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(gmax=0.010, Beta=a, mg=1.0)

    net.cells["IC"]["cells"][0].sec["soma"](0.5).pas.g = 1.0/5000.0
    net.cells["MSO_ON"]["delay"] = 10
    net.cells["MSO_OFF"]["delay"] = 5

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1
    return net



def C_RECEPTORS(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,50,1)]
        ampa_g = [i*0.001 for i in range(0,21,2)]
        nmda_g = [i*0.001 for i in range(0,21,2)]
        gaba_g = [i*0.001 for i in range(0,21,2)]
        param = []
        for a in ampa_g:
            for b in nmda_g:
                for c in gaba_g:
                        param.append([a,b,c])
        return [10,100,stims, param]

    net.cells["IC"]["cells"][0].sec["dendE"].modifyAMPA(gmax=a[0])
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyAMPA(gmax=a[0])
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=a[1],mg=1)
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(gmax=a[1],mg=1)
    net.cells["IC"]["cells"][0].sec["dendI"].modifyGABAa(gmax=a[2])
    return net

def C_PHYSICAL(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,26)]
        soma_size = [i for i in range(5,26,2)]
        onset_length = [1]+[i for i in range(100,1001,100)]
        amplitude = [i*0.01+0.03 for i in range(0,8,1)]
        param = []
        for a in soma_size:
            for b in onset_length:
                for c in amplitude:
                        param.append([a,b,c])
        return [10,100,stims, param]

    net.cells["IC"]["cells"][0].sec["soma"].L=a[0]
    net.cells["IC"]["cells"][0].sec["soma"].diam=a[0]
    net.cells["IC"]["cells"][0].sec["dendE"].L=a[1]
    net.sim_amp=a[2]
    return net

def C_SEARCH_RATIOS(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,26)]
        nmda_gmax = [i*0.1*0.01 for i in range(0,21,4)]
        nmda_beta = [i*0.1*0.0066 for i in range(0,21,2)]
        inhib_gmax = [i*0.1*0.002 for i in range(0,21,5)]
        mg = [i*0.5 for i in range(0,5,1)]
        param = []
        for a in nmda_gmax:
            for b in nmda_beta:
                for c in inhib_gmax:
                    for d in mg:
                        param.append([a,b,c,d])
        return [10,100,stims, param]

    net.cells["MSO_ON"]["delay"] = 15
    net.cells["MSO_OFF"]["delay"] = 2
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(gmax=a[0], Beta=a[1], mg=a[3])
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=a[0], Beta=a[1], mg=a[3])
    net.cells["IC"]["cells"][0].sec["dendI"].modifyGABAa(gmax=a[2])
    return net

def C_SEARCH(net,a,stim,getparams=False):
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
        return [10,100,stims, param]

    net.cells["MSO_ON"]["delay"] = a[0]
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(gmax=0.01*a[1], mg=0.5)
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyAMPA(gmax=0.005*a[1])
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=0.01*a[2], mg=0.5)
    net.cells["IC"]["cells"][0].sec["dendE"].modifyAMPA(gmax=0.005*a[2])
    return net

def AC_NMDA_BETA(net,a,stim,getparams=False):
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(Beta=a, mg=0.5)
    return net

def C_NMDA_BETA(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,25)] + [i for i in range(25,251,5)]
        param = [i*0.002 for i in range(1,22,1)]
        return [10,100,stims, param]
    net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(Beta=a, mg=0.5)
    net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(Beta=a, mg=0.5)
    return net
