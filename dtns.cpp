#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "boost/threadpool.hpp"
#include <boost/math/distributions/normal.hpp>
#include <boost/random.hpp>
#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include "GLE.h"
#include "simulation.h"
#include "synapse.h"
#include <vector>
#include <numeric>
#include <math.h>

namespace po = boost::program_options;

bool progress(boost::threadpool::pool &threads, int total) 
{

    int left = threads.pending();
    int done = total - threads.pending();
    static boost::progress_display show_progress( total );

    if (left == 0) {
        show_progress += total - show_progress.count();
        std::cout << std::endl;
        return false;
    }

    show_progress += done - show_progress.count();

    /*
    cout << "\r[" << (int) (percent_done * 100) << "%] ";
    cout << "[" << pending << "/" << total << "] ";
    cout << "[" << (active - 1) << " active] ";
    cout << left << " remaining." << flush;
    */
    return true;
}

int main(int argc, char* argv[]) 
{

    // Determine the program options and set default values
    int nThreads = 4;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("threads", po::value<int>(), "set number of concurrent events")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if(vm.count("threads")) {
        nThreads = vm["threads"].as<int>();
    }



	std::cout << "Initializing Simulations... " << std::flush;
    // Define iterators for use later
	std::vector<Simulation>::iterator simit;
	std::vector< std::vector<Simulation> >::iterator simsimit;
	std::vector< std::vector< std::vector<Simulation> > >::iterator simsimsimit;

    // 3D Array for simulations. [Parameter][Input Duration][Repeated Trials] 
	std::vector< std::vector< std::vector<Simulation> > > trials;
	std::vector< std::vector<Simulation> > durations;
	std::vector<Simulation> sims;

    // 2D Array for Surface Plot of Average Spike Counts over 2 Dimensions
    std::vector< std::vector< double > > averagedResults;
    std::vector<double> simY;
    std::vector<double> simX;

	double sS, sE, sI;
	int repeats = 5;
	sS = 1;
	sE = 200;
	sI = 2;

    boost::mt19937 rng;
    boost::normal_distribution<> nd(0.0, 1.5);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
    
    // Seed with current time
    rng.seed(static_cast<unsigned int>(std::time(0)));

    double tOn1,tOn2,tOff1,tOff2,tS1,tS2,dOn,dOff,gMaxOn,gMaxOff,gMaxS;
    tOn1 = tOn2 = 1.1;
    tOff1 = tOff2 = 1.1;
    tS1 = 1;
    tS2 = 5;
    dOn = 100;
    dOff = 3;
    gMaxOn = 10;
    gMaxOff = 10;
    gMaxS = 0;

    bool normalize = true;

    unsigned int numSims = 0;

    simX.clear();
    for (double p = 0; p <= 1; p += 0.05)
    {
        gMaxOn = gMaxOff= 2;
        gMaxS = p*50;

        simX.push_back(p);

        durations.clear();

        simY.clear();
        for (int i = sS; i <= sE; i+=sI)
        {
            simY.push_back(i);
            sims.clear();
            for (int repeat = 0; repeat < repeats; ++repeat)
            {
                Simulation sim;
                sim.C = 200;
                sim.useVoltage = false;
                sim.defaultparams();

                Synapse OnE;
                Synapse OffE;
                Synapse SusI;
                OnE.gMax = gMaxOn;
                OnE.tau1 = tOn1;
                OnE.tau2 = tOn2;
                OnE.del = dOn;
                OffE.gMax = gMaxOff;
                OffE.tau1 = tOff1;
                OffE.tau2 = tOff2;
                OffE.del = dOff;
                SusI.E = -75;
                SusI.tau1 = tS1;
                SusI.tau2 = tS2;
                SusI.gMax = gMaxS;

                OnE.spikes.push_back(10/*+var_nor()*/);
                OffE.spikes.push_back(10+i/*+var_nor()*/);
                for (int j = 10; j<=10+i; j++)
                {
                        SusI.spikes.push_back(j);
                }
                sim.synapses.push_back(OnE);
                sim.synapses.push_back(OffE);
                sim.synapses.push_back(SusI);
                sims.push_back(sim);
                ++numSims;
            }
            durations.push_back(sims);
        }
        trials.push_back(durations);
    }
    std::cout << numSims << " prepared... [OK]" << std::endl;

    std::cout << "Pooling " << nThreads << " threads... " << std::flush;
	boost::threadpool::thread_pool<> threads(nThreads+1); // Add one for the looped task func
    std::cout << "[OK]" << std::endl;
	std::cout << "Running threads... " << std::flush;
    threads.schedule(boost::threadpool::looped_task_func(boost::bind(progress, threads, numSims), 250));
    for (simsimsimit = trials.begin(); simsimsimit < trials.end(); simsimsimit++)
    {
        for (simsimit = simsimsimit->begin(); simsimit < simsimsimit->end(); simsimit++)
        {
            for (simit = simsimit->begin(); simit < simsimit->end(); simit++)
            {
                threads.schedule(boost::bind(&Simulation::runSim,&(*simit)));
            }
        }
    }
	threads.wait();
    std::cout << "[OK]" << std::endl;

    std::vector<double> tmp;
    std::vector<double>::iterator tmpit;
	double spikes;
    double max;
    for (simsimsimit = trials.begin(); simsimsimit < trials.end(); simsimsimit++)
    {
        tmp.clear();
        for (simsimit = simsimsimit->begin(); simsimit < simsimsimit->end(); simsimit++)
        {
            spikes = 0;
            for (simit = simsimit->begin(); simit < simsimit->end(); simit++)
            {
                spikes += simit->spikes().size();
            }
            spikes /= repeats;
            tmp.push_back(spikes);
        }
        max = *(std::max_element(tmp.begin(), tmp.end()));
        if (max == 0.0 || !normalize) max = 1;
        for (tmpit = tmp.begin(); tmpit < tmp.end(); tmpit++)
        {
            *tmpit /= max;
        }
        averagedResults.push_back(tmp);
    }

	GLE gle;
	GLE::PlotProperties plotProperties;
	GLE::PanelProperties panelProperties;
	GLE::Color start;
	GLE::Color end;
	GLE::PanelID panelID = GLE::NEW_PANEL;
	plotProperties.pointSize = 0;
    plotProperties.usemap = true;
    panelID = gle.plot3d(simY, simX, averagedResults, plotProperties);
	gle.canvasProperties.width = 6;
	gle.canvasProperties.height = 6;
	gle.draw("temp.pdf");

    return 0;
}
