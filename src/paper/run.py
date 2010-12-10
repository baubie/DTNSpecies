from sim import Simulation
import matplotlib.pyplot as plt
import network
import neuron

# Create our network
DTN_AC = network.DTN_AntiCoincidence()
DTN_C = network.DTN_Coincidence()

# Play with the parameters
#DTN_AC.cells["IC"]["cells"][0].dendI.modifyGABAa(Cmax=1, Cdur=1, Alpha=5, Beta=0.18, Erev=-80, Prethresh=0, Deadtime=1)


def show(network, cells):
    plt.subplot(len(cells)+1,1,1)
    network.cells["IC"]["cells"][0].soma.show("soma")
    '''
    network.cells["IC"]["cells"][0].dendE.show("dendE")
    network.cells["IC"]["cells"][0].dendEOff.show("dendEOff")
    network.cells["IC"]["cells"][0].dendI.show("dendI")
    plt.legend()
    '''

    count = 1
    for pop in cells:
        count = count + 1
        plt.subplot(len(cells)+1,1,count)
        for c in network.cells[pop]["cells"]:
            c.soma.show()

    plt.show()


# Initialize the simulation with the network
s = Simulation(DTN_C)



# Run the simulations
for d in [8]:
    s.stim_dur = d
    s.sim_time = 100
    s.run()
    show(DTN_C, ["MSO_ON", "MSO_OFF", "DNLL"])

neuron.h.quit()
