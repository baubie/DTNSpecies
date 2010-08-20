#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "boost/threadpool.hpp"
#include <boost/math/distributions/normal.hpp>
#include <boost/random.hpp>
#include "GLE.h"
#include "simulation.h"
#include "synapse.h"
#include <vector>

int main(int argc, char* argv[]) 
{

	std::cout << "Initializing Simulation..." << std::endl << std::flush;
	std::vector< std::vector<Simulation> > sims;
	std::vector<Simulation>::iterator simit;
	std::vector< std::vector<Simulation> >::iterator simsimit;

	std::vector< std::vector< std::vector<Simulation> > > trials;
	std::vector< std::vector< std::vector<Simulation> > >::iterator trialit;

	std::vector<double> simNames;
	std::vector<Simulation> simtau;
	double sS, sE, sI;
	int repeats = 1;
	sS = 1;
	sE = 100;
	sI = 1;
	trials.reserve(repeats);

	boost::mt19937 rng; // I don't seed it on purpouse (it's not relevant)
	boost::normal_distribution<> nd(0.0, 1.5);
	boost::variate_generator<boost::mt19937&, 
						   boost::normal_distribution<> > var_nor(rng, nd);
	

	for (int repeat = 0; repeat < repeats; ++repeat)
	{
		sims.clear();
		for (double tau = 1; tau <= 10; tau +=1.0)
		{
			simtau.clear();
			simNames.clear();
			for (int i = sS; i <= sE; i+=sI)
			{
				double gMax = 10;
				simNames.push_back((double)i);
				Simulation sim;
				sim.C = 200 + (tau-1)*10;
				sim.useVoltage = true;
				sim.defaultparams();
				Synapse OnE;
				Synapse OffE;
				Synapse SusI;
				OnE.gMax = gMax;
				OnE.tau1 = tau;
				OnE.tau2 = tau;
				OnE.del = 15;
				OffE.gMax = gMax;
				OffE.tau1 = tau;
				OffE.tau2 = tau;
				OffE.del = 3;
				SusI.E = -75;
				SusI.tau1 = 4;
				SusI.tau2 = 6;
				SusI.gMax = 0;	

				OnE.spikes.push_back(10+var_nor());
				OffE.spikes.push_back(10+i+var_nor());
				for (int j = 10; j<=10+i; j++)
				{
					SusI.spikes.push_back(j);
				}
				sim.synapses.push_back(OnE);
				sim.synapses.push_back(OffE);
				sim.synapses.push_back(SusI);

				simtau.push_back(sim);
			}
			sims.push_back(simtau);
		}
		trials.push_back(sims);
	}

	std::cout << "Running threads..." << std::endl;
	boost::threadpool::thread_pool<> threads(2);
	for (trialit = trials.begin(); trialit < trials.end(); trialit++)
	{
		for (simsimit = trialit->begin(); simsimit < trialit->end(); simsimit++)
		{
			for (simit = simsimit->begin(); simit < simsimit->end(); simit++)
			{
				threads.schedule(boost::bind(&Simulation::runSim,&(*simit)));
			}
		}
	}
	threads.wait();

	std::vector<double> time = trials[0][0][0].timesteps();
	std::vector< std::vector<double> > y,tmp;
	std::vector<double> spikes;
	bool first = true;
	for (trialit = trials.begin(); trialit < trials.end(); trialit++)
	{
		tmp.clear();
		for (simsimit = trialit->begin(); simsimit < trialit->end(); simsimit++)
		{
			spikes.clear();
			for (simit = simsimit->begin(); simit < simsimit->end(); simit++)
			{
				spikes.push_back((double)simit->spikes().size()/repeats);
			}
			tmp.push_back(spikes);
		}
		if (first) {
			y = tmp;
			first = false;
		} else {
			for (unsigned int i = 0; i < tmp.size(); ++i)
			{
				for (unsigned int j = 0; j < tmp[0].size(); ++j)
				{
					y[i][j] += tmp[i][j];
				}
			}
		}
	}
	y.clear();
	y.push_back(trials[0][0][0].voltagetrace());

	GLE gle;
	GLE::PlotProperties plotProperties;
	GLE::PanelProperties panelProperties;
	GLE::Color start;
	GLE::Color end;
	GLE::PanelID panelID;
	plotProperties.pointSize = 0;
	for (unsigned int i = 0; i < y.size(); ++i)
	{
		//panelID = gle.plot(simNames,y[i], plotProperties);
		panelID = gle.plot(time,y[i], plotProperties);
		gle.setPanelProperties(panelProperties, panelID);
	}
	gle.canvasProperties.width = 10;
	gle.canvasProperties.height = 4*y.size();

	gle.draw("temp.pdf");
	

    return 0;
}
