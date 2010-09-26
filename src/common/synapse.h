#ifndef SYNAPSE_H
#define SYNAPSE_H

#include <math.h>
#include <vector>
#include <boost/unordered_map.hpp>
#include <utility>

#include <iostream>

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

            static void prepare(double tau1, double tau2);
            static double dt;

            std::vector<double> spikes;

	private:
            // Synapatic alpha function
            double alpha(double t);
            static boost::unordered_map<double,double> cache;
};


#endif
