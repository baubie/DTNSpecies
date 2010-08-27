#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "boost/threadpool.hpp"
#include <boost/math/distributions/normal.hpp>
#include <boost/random.hpp>
#include <boost/program_options.hpp>
#include <boost/progress.hpp>
#include <vector>
#include <numeric>
#include <math.h>

#include "GLE.h"
#include "simulation.h"
#include "synapse.h"
#include "tuning.h"
#include "wvector.h"

namespace po = boost::program_options;

bool progress(unsigned int done, unsigned int total)
{
    static boost::progress_display show_progress( total );
    if (done == total) {
        show_progress += total - show_progress.count();
        std::cout << std::endl;
        return false;
    }
    show_progress += done - show_progress.count();
    return true;
}



int main(int argc, char* argv[]) 
{

    /**********************************************************************/
    /* SETUP THE PROGRAM */
    /**********************************************************************/
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
    /**********************************************************************/



    std::cout << "Initializing Simulations... " << std::flush;
    boost::threadpool::thread_pool<> threads(nThreads);

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
    int repeats = 1;
    sS = 1;
    sE = 100;
    sI = 2;

    boost::mt19937 rng;
    boost::normal_distribution<> nd(0.0, 1.5);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
    
    // Seed with current time
    rng.seed(static_cast<unsigned int>(std::time(0)));

    bool normalize = true;
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

    wvector<double> wv_tau, wv_dOn, wv_dOff, wv_gMaxOn, wv_gMaxOff;

    std::vector<double> tauRec;

    // Fill wvectors with possible values
    for (double i = 0.2; i <= 20; i+=0.2)
    {
        wv_tau.push_back(i);
    }
    for (double i = 1; i <= 200; i+=1.0)
    {
        wv_dOn.push_back(i);
        wv_dOff.push_back(i);
    }
    for (double i = 0; i <= 30; i+=0.5)
    {
        wv_gMaxOn.push_back(i);
        wv_gMaxOff.push_back(i);
    }

    std::cout << "[OK]" << std::endl;

    unsigned int numIterations = 50;
    unsigned int numSimsPerIteration =  ((sE-sS)/sI + 1)*repeats;
    unsigned int numSims = numIterations * numSimsPerIteration;
    unsigned int done = 0;

    simX.clear();
    std::cout << "Running " << numSims << " Simulations... " << std::flush;
    for (unsigned int iteration = 0; iteration < numIterations; ++iteration)
    {
        progress(done, numSims);
        gMaxOn = *(wv_gMaxOn.random());
        gMaxOff = *(wv_gMaxOff.random());
        tOn1 = tOn2 = tOff1 = tOff2 = *(wv_tau.random());
        dOn = *(wv_dOn.random());
        dOff = *(wv_dOff.random());
        tauRec.push_back(tOn1);

        simY.push_back(iteration);
        durations.clear();

        simX.clear();
        for (int i = sS; i <= sE; i+=sI)
        {
            simX.push_back(i);
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

        for (simsimit = durations.begin(); simsimit < durations.end(); simsimit++)
        {
            for (simit = simsimit->begin(); simit < simsimit->end(); simit++)
            {
                threads.schedule(boost::bind(&Simulation::runSim,&(*simit)));
            }
        }
        threads.wait();
        done += numSimsPerIteration;
        trials.push_back(durations);
    }
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
	GLE::PanelID panelID;
	plotProperties.pointSize = 0;
        plotProperties.usemap = true;
        panelID = gle.plot3d(simX, simY, averagedResults, plotProperties);
        panelID = gle.plot(simX, tauRec, plotProperties);
	gle.canvasProperties.width = 6;
	gle.canvasProperties.height = 6;
	gle.draw("temp.pdf");

    return 0;
}
