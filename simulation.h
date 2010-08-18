#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <math.h>
#include <boost/thread.hpp>
#include <vector>
#include "synapse.h"
#include <boost/math/distributions/normal.hpp>
#include <boost/random.hpp>

class Simulation
{
    public:
		// Setup
		void defaultparams();
        void runSim();

		// Generate timestep vector
		std::vector<double> timesteps();

		// Generate voltage trace vector
		std::vector<double> voltagetrace();
		std::vector<double> spikes();

		// Synapses
		std::vector<Synapse> synapses;

    private:

        // Simulation parameters
        double dt;
		double T;
		double del;

        // Model state variables
        double V;
        double w;
		std::vector<double> m_Vstored;
		std::vector<double> m_Spikes;

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

        // V = V + dt*dV
        double dV(double V, double I);
		double dw(double w);

};

#endif
