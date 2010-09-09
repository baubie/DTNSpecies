#include <iostream>
#include <fstream>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "boost/threadpool.hpp"
#include <boost/math/distributions/normal.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/random.hpp>
#include <boost/program_options.hpp>
#include <vector>
#include <map>
#include <numeric>
#include <math.h>
#include <ncurses.h>

#include "GLE.h"
#include "simulation.h"
#include "synapse.h"
#include "tuning.h"
#include "vectors.h"

namespace po = boost::program_options;
using namespace boost::posix_time;
using namespace boost::gregorian;

bool progress(long done, long total)
{
    static ptime start = second_clock::local_time();
    std::string endTime;
    time_duration diff, total_time;

    if (done > 0) {
        ptime now = second_clock::local_time();
        diff = now - start;
        total_time = (now-start) * (total / done);
        ptime finish = start + total_time;
        endTime = to_simple_string(finish);
    } else { endTime = "Unknown"; } 

    int row,col;
    getyx(stdscr,row,col);
    double percent = (double)(done/total);

    clrtoeol();
    mvprintw(LINES-2,0,"%d/%d (%.5f\%) Time Left: ", total-done, total, percent);
    printw(to_simple_string(total_time-diff).c_str());
    printw(" Estimated End Time: ");
    printw(endTime.c_str());
    refresh();
    if (done == total) {
        return false;
    }
    return true;
}



int main(int argc, char* argv[]) 
{


    /**********************************************************************/
    /* SETUP THE PROGRAM */
    /**********************************************************************/
    // Determine the program options and set default values
    int nThreads = 4;
    int repeats = 1;
    bool normalize = false;
    std::string logFile = "log";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("threads,t", po::value<int>(), "set number of concurrent events")
        ("normalize,n", po::value<bool>()->implicit_value(true), "normalize the plot")
        ("repeats,r", po::value<int>(), "number of times to repeat each trial")
        ("logfile,l", po::value<std::string>(), "log file")
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
    if(vm.count("normalize")) {
        normalize = true;
    }
    if(vm.count("repeats")) {
        repeats = vm["repeats"].as<int>();
    }
    if(vm.count("logfile")) {
        logFile = vm["logfile"].as<std::string>();
    }
    /**********************************************************************/


    // Setup NCURSES
    initscr();

    printw("Network Iterator - Version 0.9\n");
    refresh();

    std::ofstream oflog(logFile.c_str());
    std::clog.rdbuf(oflog.rdbuf());

    printw("Logging results in %s\n", logFile.c_str());
    printw("Initializing simulations...\n");
    refresh();
    boost::threadpool::thread_pool<> threads(nThreads);

    // Define iterators for use later
    std::vector<Simulation>::iterator simit;
    std::vector< std::vector<Simulation> >::iterator simsimit;
    std::vector< std::vector< std::vector<Simulation> > >::iterator simsimsimit;

    // 3D Array for simulations. [Parameter][Input Duration][Repeated Trials] 
    std::vector< std::vector< std::vector<Simulation> > > trials;
    std::vector< std::vector<Simulation> > durations;
    std::vector<Simulation> sims;


    double sS, sE, sI;
    sS = 10;
    sE = 200;
    sI = 10;

    boost::mt19937 rng;
    boost::normal_distribution<> nd(0.0, 1.5);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
    
    // Seed with current time
    rng.seed(static_cast<unsigned int>(std::time(0)));

    double tOn1,tOn2,tOff1,tOff2,tS1,tS2,dOn,dOff,dIOn,gMaxOn,gMaxOff,gMaxS;
    tOn1 = tOn2 = 1.1;
    tOff1 = tOff2 = 1.1;
    tS1 = 0.5;
    tS2 = 2;
    dOn = 100;
    dOff = 3;
    dIOn = 0;
    gMaxOn = 10;
    gMaxOff = 10;
    gMaxS = 0;

    vectors<double> params;
    std::vector<double> param;
    std::vector<double> recVal;


    for (double i = 1.0; i <= 21; i+=5.0)
    {
        param.push_back(i);
    }
    params.add("tauOn", param);
    params.add("tauOff", param);
    param.clear();
    for (double i = 0; i <= 150; i+=25.0)
    {
        param.push_back(i);
    }
    params.add("dOn", param);
    params.add("dIOn", param);
    param.clear();
    /*
    for (double i = 1; i <= 10; i+=1)
    {
        param.push_back(i);
    }
    params.add("dOff", param);
    param.clear();
    */
    for (double i = 8; i <= 10; i+=2.0)
    {
        param.push_back(i);
    }
    params.add("gMaxOn", param);
    params.add("gMaxOff", param);
//    params.add("gMaxI", param);
    param.clear();


    Tuning bp(sS,sE,sI);
    /* Bat Bandpass 
    bp.define(1,0,0.5);
    bp.define(2,0,0.9);
    bp.define(6,2);
    bp.define(12,0,0.9);
    bp.define(13,0,0.5);
    bp.define(20,0,0.5);
    bp.define(25,0,0.5);
    */

    /* Rat Bandpass */
    bp.define(50,2);
    bp.define(5,0,0.5);
    bp.define(100,0,0.5);
    bp.smooth();
    std::clog << bp.print() << std::flush;

    long numSims = repeats * ((sE-sS)/sI+1) * params.size();
    long done = 0;
    std::map<double,double> trialResults;

    double maxScore = 0;
    std::vector<int> goodTrials; // Any searches that yield results store here

    printw("Search through %d networks  over %d parameters.\n", numSims, params.size());
    refresh();

    for (bool searchMode = true; searchMode != false; searchMode = !searchMode)
    {
        for (; !params.done(); params++)
        {
            if (done % 50 == 0)
            {
                progress(done, numSims);
            }

            gMaxOn = params.val("gMaxOn");
            gMaxOff = params.val("gMaxOff");;
            tOn1 = tOn2 = params.val("tauOn");
            tOff1 = tOff2 = params.val("tauOff");
            dOn = params.val("dOn");
            //dOff = params.val("dOff");

            durations.clear();

            trialResults.clear();
            for (int i = sS; i <= sE; i+=sI)
            {
                sims.clear();
                for (int repeat = 0; repeat < repeats; ++repeat)
                {
                    Simulation sim;
                    sim.C = 180;
                    sim.useVoltage = false;
                    sim.defaultparams();
                    sim.T = 333;

                    Synapse OnE;
                    Synapse OffE;
                    Synapse SusI;
                    OnE.gMax = gMaxOn;
                    OnE.tau1 = 1.0;
                    OnE.tau2 = tOn2;
                    OnE.del = dOn;
                    OffE.gMax = gMaxOff;
                    OffE.tau1 = 1.0;
                    OffE.tau2 = tOff2;
                    OffE.del = dOff;
                    SusI.E = -75;
                    SusI.del = dIOn;
                    SusI.tau1 = tS1;
                    SusI.tau2 = tS2;
                    SusI.gMax = gMaxS;

                    OnE.spikes.push_back(0+var_nor());
                    OffE.spikes.push_back(i+var_nor());
                    for (int j = 0; j<=+i; ++j)
                    {
                        SusI.spikes.push_back(j);
                    }
                    sim.synapses.push_back(OnE);
                    sim.synapses.push_back(OffE);
                    sim.synapses.push_back(SusI);
                    sims.push_back(sim);
                    ++done;
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

            // Extract results
            double spikes;
            for (simsimit = durations.begin(); simsimit < durations.end(); simsimit++)
            {
                spikes = 0;
                for (simit = simsimit->begin(); simit < simsimit->end(); simit++)
                {
                    spikes += simit->spikes().size();
                }
                spikes /= repeats;
                trialResults[std::distance(durations.begin(), simsimit)*sI+sS] = spikes;
            }
            double score = bp.score(trialResults);

            if (score > maxScore) {
                maxScore = score;
                std::clog << "MAX (" << score << ") [";
                std::clog << "gMaxOn: " << gMaxOn << ", ";
                std::clog << "gMaxOff: " << gMaxOff << ", ";
                std::clog << "gMaxS: " << gMaxS << ", ";
                std::clog << "tauOn: " << tOn1 << ", ";
                std::clog << "tauOff: " << tOff1 << ", ";
                std::clog << "dOn: " << dOn << ", ";
                std::clog << "dOff: " << dOff << ", ";
                std::clog << "dIOn: " << dIOn << "]" << std::endl;

                // Print it out
                std::clog << "[ ";
                for (unsigned int i = 0; i < trialResults.size(); ++i)
                {
                    std::clog << trialResults[i] << "\t";
                }
                std::clog << "]" << std::endl;
                std::clog << std::endl;
                std::clog << std::flush;
            }
            mvprintw(LINES-3, 0, "High Score: %.5f", maxScore);
            refresh();
            recVal.push_back(score);
        }
    }

/*
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
    }

    // Find the maximum weight
    // Use that to find the best output
    std::vector<double>::iterator maxWeight = std::max_element(recVal.begin(), recVal.end()); 
    unsigned int pos = std::distance(recVal.begin(), maxWeight);
    std::vector<double> bestOutput = averagedResults[pos];
    // Print it out
    std::cout << "Best Score: " << *maxWeight << " at position " << pos << std::endl;
    std::cout << "[ ";
    for (unsigned int i = 0; i < bestOutput.size(); ++i)
    {
        std::cout << bestOutput[i] << "\t";
    }
    std::cout << "]" << std::endl;
    bp.print();

	GLE gle;
	GLE::PlotProperties plotProperties;
	GLE::PanelProperties panelProperties;
	GLE::PanelID panelID;
	plotProperties.pointSize = 0;
    plotProperties.usemap = true;
    panelID = gle.plot3d(simX, simY, averagedResults, plotProperties);
    panelID = gle.plot(simY, recVal, plotProperties);
	gle.canvasProperties.width = 10;
	gle.canvasProperties.height = 20;
	gle.draw("temp.pdf");
*/

    endwin();

    return 0;
}
