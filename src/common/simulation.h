#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <boost/thread.hpp>
#include <vector>
#include "synapse.h"

class Simulation
{
    public:
        void defaultparams();
        void runSim();

        // Generate timestep vector
        std::vector<double> timesteps();

        // Generate voltage trace vector
        std::vector<double> voltagetrace();
        std::vector<double> spikes();

        // Synapses
        std::vector<Synapse> synapses;

        // Model neuron parameters
        double gL;
        double EL;
        double a;
        double b;
        double C;
        double dT;
        double VT;
        double Vr;
        double tau_w;

        double T;
        double del;
        double dt;
        bool useVoltage;

    private:
        // Model state variables
        double V;
        double w;
        std::vector<double> m_Vstored;
        std::vector<double> m_Spikes;

        // V = V + dt*dV
        double dV(double V, double I);
        double dw(double w);

};

#endif
