#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "boost/threadpool.hpp"
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
	std::vector<double> simNames;
	std::vector<Simulation> simtau;
	double sS, sE, sI;
	sS = 1;
	sE = 200;
	sI = 1;

	for (double tau = 1; tau <= 5; tau +=0.5)
	{
		simtau.clear();
		simNames.clear();
		for (int i = sS; i <= sE; i+=sI)
		{
			double gMax = 10.5-tau*1.5;
			simNames.push_back((double)i);
			Simulation sim;
			sim.defaultparams();
			Synapse OnE;
			Synapse OffE;
			Synapse SusI;
			OnE.gMax = gMax;
			OnE.tau1 = tau;
			OnE.tau2 = tau;
			OnE.del = 10+15*tau;
			OffE.gMax = gMax;
			OffE.tau1 = tau;
			OffE.tau2 = tau;
			OffE.del = 3;
			SusI.E = -75;
			SusI.tau1 = 4;
			SusI.tau2 = 6;
			SusI.gMax = 0;	

			OnE.spikes.push_back(10);
			OffE.spikes.push_back(10+i);
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

	std::cout << "Running threads..." << std::endl;
	boost::threadpool::thread_pool<> threads(2);
	for (simsimit = sims.begin(); simsimit < sims.end(); simsimit++)
	{
		for (simit = simsimit->begin(); simit < simsimit->end(); simit++)
		{
			threads.schedule(boost::bind(&Simulation::runSim,&(*simit)));
		}
	}
	threads.wait();

//	std::vector<double> time = sims[0].timesteps();
	std::vector< std::vector<double> > y;
	std::vector<double> spikes;
	for (simsimit = sims.begin(); simsimit < sims.end(); simsimit++)
	{
		spikes.clear();
		for (simit = simsimit->begin(); simit < simsimit->end(); simit++)
		{
			spikes.push_back(simit->spikes().size());
		}
		y.push_back(spikes);
	}

	GLE gle;
	GLE::PlotProperties plotProperties;
	GLE::PanelProperties panelProperties;
	GLE::Color start;
	GLE::Color end;
	GLE::PanelID panelID;
	plotProperties.pointSize = 0;
	panelID = gle.plot(simNames,y, plotProperties);
	gle.setPanelProperties(panelProperties, panelID);
	gle.canvasProperties.width = 10;
	gle.canvasProperties.height = 4;
	gle.draw("temp.pdf");

    return 0;
}
