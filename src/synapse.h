#ifndef SYNAPSE_H
#define SYNAPSE_H

#include <math.h>
#include <vector>
#include "fastexp.h"

class Synapse
{
	public:
		// Constructor
		Synapse();

		// Parameters
		double tau1;
		double tau2;
		double gMax;
		double del;
		double E;

		// Get Conductance 
		double g(double t);
		double I(double t,double V);

		std::vector<double> spikes;

	private:
        // Synapatic alpha function
        double alpha(double t);

};

#endif
