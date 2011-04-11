from sim import Simulation
import network
import neuron
import progress
from math import ceil


def run(netdef,tosave,modify,procs,thisProc,stims,param,repeats,sim_time,SaveSpikes,SaveVoltage,SaveConductance,SaveCurrent):
    net = netdef()

    if SaveVoltage:
        net.recordVoltage()

    repeats = int(repeats)
    # Randomseed was 200 for most figures
    # Changed to 200 for rat
    s = Simulation(net, randomseed=200,delay=25)
    s.verbose = False
    s.sim_time = sim_time
    s.dt = 0.050
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
                net.savecells(tosave, key, spikes=SaveSpikes,voltage=SaveVoltage,conductance=SaveConductance,current=SaveCurrent)
            count += 1
    progress.update(spp,spp,thisProc)

    r = [thisProc,net.savedparams,net.savedcells]
    return r


def C_DEFAULT(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,26,1)]
        param = [1]
        return [20,100,stims,param]

    if stim <= 2:
        mult = 0.375*(stim)
        mult = 1.0
    else:
        mult = 1.0

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.004*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=0.020*2*mult,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=0.0025)

    net.cells["IC"]["cells"][0].sec["soma"](0.5).pas.g = 1.0/4000

    net.cells["MSO_ON"]["delay"] = 10
    net.cells["MSO_OFF"]["delay"] = 6
    net.cells["DNLL"]["delay"] = 9

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net

def C_NMDA_BETA(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,201,2)]#+[i for i in range(50,100,2)]+[i for i in range(100,251,5)];

        NMDAbeta = [0.00165,0.0033,0.0066,0.0132]
        param = []
        for b in NMDAbeta:
                param.append([a,b])
        return [5,500,stims,param]

    if stim <= 25:
        mult = 0.375*(stim)
        mult = 0.04*stim
        mult = 1
    else:
        mult = 1.0

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.004*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=(0.020+0.010)*2*mult,Beta=a[0],mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=0.0025) # Normally 0.0035

    net.cells["MSO_ON"]["delay"] = 10
    net.cells["MSO_OFF"]["delay"] = 6
    net.cells["DNLL"]["delay"] = 9

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net

def C_TAU(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,201,2)]
        tau = [2000,3000,4000,5000,6000,8000,10000]
        param = []
        for t in tau:
            param.append([t])
        return [20,500,stims,param]

    mult = 1

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.004*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=0.020*2*mult,Beta=0.0066,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"](0.5).pas.g = 1.0/a[0]
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=0.0025) # Normally 0.0035

    net.cells["MSO_ON"]["delay"] = 10
    net.cells["MSO_OFF"]["delay"] = 6
    net.cells["DNLL"]["delay"] = 9

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net

def C_GABA(net,a,stim,getparams=False):

    if getparams:
        stims = [i for i in range(1,201,5)]#+[i for i in range(50,100,2)]+[i for i in range(100,251,5)];
        inhg = [0.0,0.001, 0.0015, 0.0025, 0.0035, 0.0045]
        param = []
        for i in inhg:
            param.append([i])
        return [20,500,stims,param]

    mult = 1 

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.004*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=0.020*2*mult,Beta=0.0066,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=a[0]) # Normally 0.0035

    net.cells["MSO_ON"]["delay"] = 10 
    net.cells["MSO_OFF"]["delay"] = 6 
    net.cells["DNLL"]["delay"] = 9

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net

def C_NMDA(net,a,stim,getparams=False):

    if getparams:
        stims = [i for i in range(1,201,5)]#+[i for i in range(50,100,2)]+[i for i in range(100,251,5)];
        inhg = [0.0,0.005,0.015,0.02,0.025,0.03,0.035]
        param = []
        for i in inhg:
            param.append([i])
        return [20,500,stims,param]

    mult = 1 

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.004*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=a[0]*2*mult,Beta=0.0066,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=0.0025) # Normally 0.0035

    net.cells["MSO_ON"]["delay"] = 10 
    net.cells["MSO_OFF"]["delay"] = 6 
    net.cells["DNLL"]["delay"] = 9

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net

def C_AMPA(net,a,stim,getparams=False):

    if getparams:
        stims = [i for i in range(1,201,5)]#+[i for i in range(50,100,2)]+[i for i in range(100,251,5)];
        inhg = [0.0,0.002,0.004,0.006,0.008]
        param = []
        for i in inhg:
            param.append([i])
        return [20,500,stims,param]

    mult = 1 

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=a[0]*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=0.020*2*mult,Beta=0.0066,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=0.0025) # Normally 0.0035

    net.cells["MSO_ON"]["delay"] = 10 
    net.cells["MSO_OFF"]["delay"] = 6 
    net.cells["DNLL"]["delay"] = 9

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net

def C_ONSET(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,201,5)]#+[i for i in range(50,100,2)]+[i for i in range(100,251,5)];
        Onset = [10,15,25,50,75]
        param = []
        for a in Onset:
            param.append([a])
        return [20,500,stims,param]

    if stim <= 25:
        mult = 0.375*(stim)
        mult = 0.04*stim
        mult = 1
    else:
        mult = 1.0

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.004*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=0.020*2*mult,Beta=0.0066,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=0.0025) # Normally 0.0035

    net.cells["MSO_ON"]["delay"] = a[0]
    net.cells["MSO_OFF"]["delay"] = 6 
    net.cells["DNLL"]["delay"] = 9

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net

def C_RAT(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(5,206,10)]
        stims = [5, 15, 26, 36, 46, 56, 67, 77, 87, 97, 108, 118, 128, 138, 149, 159, 169, 180, 190, 200]
        param = [i for i in range(100)]
        #param = [1]
        return [20,500,stims,param]

    mult = 1.0

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.0030*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=0.012*2*mult,Beta=0.0066,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAaRange(range(10), gmax=0.00080/10)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAaRange(range(10,20), gmax=0.0030/10) #0.00677

    net.cells["IC"]["cells"][0].sec["soma"](0.5).pas.g = 1.0/10000

    net.cells["MSO_ON"]["delay"] = 15
    net.cells["MSO_OFF"]["delay"] = 32
    net.cells["DNLL"]["delay"] = 50
    net.cells["DNLLEarly"]["delay"] = 15
    net.cells["DNLLEarly"]["mindur"] = 35

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net

def C_BAT(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,26,1)]
        param = [1]
        return [20,150,stims,param]

    if stim <= 1:
        mult = 0.0
    else:
        mult = 1.0

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.0090*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=0.00*2*mult,Beta=0.0066,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=0.0052)

    net.cells["IC"]["cells"][0].sec["soma"](0.5).pas.g = 1.0/1500

    net.cells["MSO_ON"]["delay"] = 13
    net.cells["MSO_OFF"]["delay"] = 14
    net.cells["DNLL"]["delay"] = 13

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net

def C_ANURANS(net,a,stim,getparams=False):
    if getparams:
        stims = [2,5,10,15,20,25,30,40,50,100]
        param = [1]
        return [20,200,stims,param]

    mult = 1.0

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.005*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=0.020*2*mult,Beta=0.0066,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=0.0014)

    net.cells["IC"]["cells"][0].sec["soma"](0.5).pas.g = 1.0/5000

    net.cells["MSO_ON"]["delay"] = 50
    net.cells["MSO_OFF"]["delay"] = 0 
    net.cells["DNLL"]["delay"] = 25 

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0

    return net

def C_MOUSE_SP(net,a,stim,getparams=False):
    if getparams:
        stims = [i for i in range(1,32,2)]+[50,75,100]
        stims = [1, 3.6, 6.3, 8.9, 11.5, 14.2, 17,20,22,25,28,30,50,75,100]
        param = [1]
        return [20,250,stims,param]

    if stim <= 1:
        mult = 1.0
    else:
        mult = 1.0

    # modifyAMPA/NMDA scale by total number of receptors so we multiply by 2 since we have 2 inputs
    net.cells["IC"]["cells"][0].sec["soma"].modifyAMPA(gmax=0.0019*2*mult)
    net.cells["IC"]["cells"][0].sec["soma"].modifyNMDA(gmax=0.019*2*mult,Beta=0.0066,mg=1.0)
    net.cells["IC"]["cells"][0].sec["soma"].modifyGABAa(gmax=0.0012)

    net.cells["IC"]["cells"][0].sec["soma"](0.5).pas.g = 1.0/5000

    net.cells["MSO_ON"]["delay"] = 14
    net.cells["MSO_OFF"]["delay"] = 6
    net.cells["DNLL"]["delay"] = 12

    net.cells["MSO_ON"]["stim"] = "IClamp"
    net.cells["MSO_ON"]["stimamp"] = 0.1
    net.cells["MSO_OFF"]["stim"] = "IClamp"
    net.cells["MSO_OFF"]["stimamp"] = 0.1

    return net






# def C_NMDA_BETA_OLD(net,a,stim,getparams=False):
#     if getparams:
#         stims = [1,3,10,15,25]
#         stims = range(1,26,1)
#         param = [0.0066]
#         return [1,100,stims, param]
# 
#     if stim <= 1:
#         mult = 0.5*(stim)
#     else:
#         mult = 1.0
# 
#     net.cells["IC"]["cells"][0].sec["dendI"].modifyGABAa(gmax=0.015, Beta=0.18)
#     net.cells["IC"]["cells"][0].sec["dendE"].modifyAMPA(gmax=0.010*mult)
#     net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=0.010, Beta=a, mg=1.0)
#     net.cells["IC"]["cells"][0].sec["dendEOff"].modifyAMPA(gmax=0.010*mult)
#     net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(gmax=0.010, Beta=a, mg=1.0)
# 
#     net.cells["IC"]["cells"][0].sec["soma"](0.5).pas.g = 1.0/5000.0
#     net.cells["MSO_ON"]["delay"] = 10
#     net.cells["MSO_OFF"]["delay"] = 5
# 
#     net.cells["MSO_ON"]["stim"] = "IClamp"
#     net.cells["MSO_ON"]["stimamp"] = 0.1
#     net.cells["MSO_OFF"]["stim"] = "IClamp"
#     net.cells["MSO_OFF"]["stimamp"] = 0.1
#     return net
# 
# def C_RECEPTORS(net,a,stim,getparams=False):
#     if getparams:
#         stims = [i for i in range(1,50,1)]
#         ampa_g = [i*0.001 for i in range(0,21,2)]
#         nmda_g = [i*0.001 for i in range(0,21,2)]
#         gaba_g = [i*0.001 for i in range(0,21,2)]
#         param = []
#         for a in ampa_g:
#             for b in nmda_g:
#                 for c in gaba_g:
#                         param.append([a,b,c])
#         return [10,100,stims, param]
# 
#     net.cells["IC"]["cells"][0].sec["dendE"].modifyAMPA(gmax=a[0])
#     net.cells["IC"]["cells"][0].sec["dendEOff"].modifyAMPA(gmax=a[0])
#     net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=a[1],mg=1)
#     net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(gmax=a[1],mg=1)
#     net.cells["IC"]["cells"][0].sec["dendI"].modifyGABAa(gmax=a[2])
#     return net
# 
# def C_PHYSICAL(net,a,stim,getparams=False):
#     if getparams:
#         stims = [i for i in range(1,26)]
#         soma_size = [i for i in range(5,26,2)]
#         onset_length = [1]+[i for i in range(100,1001,100)]
#         amplitude = [i*0.01+0.03 for i in range(0,8,1)]
#         param = []
#         for a in soma_size:
#             for b in onset_length:
#                 for c in amplitude:
#                         param.append([a,b,c])
#         return [10,100,stims, param]
# 
#     net.cells["IC"]["cells"][0].sec["soma"].L=a[0]
#     net.cells["IC"]["cells"][0].sec["soma"].diam=a[0]
#     net.cells["IC"]["cells"][0].sec["dendE"].L=a[1]
#     net.sim_amp=a[2]
#     return net
# 
# def C_SEARCH_RATIOS(net,a,stim,getparams=False):
#     if getparams:
#         stims = [i for i in range(1,26)]
#         nmda_gmax = [i*0.1*0.01 for i in range(0,21,4)]
#         nmda_beta = [i*0.1*0.0066 for i in range(0,21,2)]
#         inhib_gmax = [i*0.1*0.002 for i in range(0,21,5)]
#         mg = [i*0.5 for i in range(0,5,1)]
#         param = []
#         for a in nmda_gmax:
#             for b in nmda_beta:
#                 for c in inhib_gmax:
#                     for d in mg:
#                         param.append([a,b,c,d])
#         return [10,100,stims, param]
# 
#     net.cells["MSO_ON"]["delay"] = 15
#     net.cells["MSO_OFF"]["delay"] = 2
#     net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(gmax=a[0], Beta=a[1], mg=a[3])
#     net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=a[0], Beta=a[1], mg=a[3])
#     net.cells["IC"]["cells"][0].sec["dendI"].modifyGABAa(gmax=a[2])
#     return net
# 
# def C_SEARCH(net,a,stim,getparams=False):
#     if getparams:
#         stims = [i for i in range(1,30)] + [i for i in range(30,251,10)]
#         ondelay = [i for i in range(5,51,5)]
#         offweight = [i*0.1 for i in range(0,21,2)]
#         onweight = [i*0.1 for i in range(0,21,2)]
#         param = []
#         for a in ondelay:
#             for b in offweight:
#                 for c in onweight:
#                     param.append([a,b,c])
#         return [10,100,stims, param]
# 
#     net.cells["MSO_ON"]["delay"] = a[0]
#     net.cells["IC"]["cells"][0].sec["dendEOff"].modifyNMDA(gmax=0.01*a[1], mg=0.5)
#     net.cells["IC"]["cells"][0].sec["dendEOff"].modifyAMPA(gmax=0.005*a[1])
#     net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(gmax=0.01*a[2], mg=0.5)
#     net.cells["IC"]["cells"][0].sec["dendE"].modifyAMPA(gmax=0.005*a[2])
#     return net
# 
# def AC_NMDA_BETA(net,a,stim,getparams=False):
#     net.cells["IC"]["cells"][0].sec["dendE"].modifyNMDA(Beta=a, mg=0.5)
#     return net
