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
#include <algorithm>
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

void progress(ptime start, long done, long total, int found, bool searchMode, int row)
{
    // Update at most twice a second for sanity sake
    static ptime last;

    if ((microsec_clock::local_time() - last).total_microseconds() < 250000) return;
    last = microsec_clock::local_time();


    std::string endTime, timeLeft, timeElapsed;
    double percent = ((double)done/(double)total);
    long seconds_left = 0;

    if (done > 0) {
        time_duration elapsed = second_clock::local_time()-start;
        seconds_left = elapsed.total_seconds()*(1/percent-1);
        time_duration left = seconds(seconds_left);
        ptime finish = second_clock::local_time() + left;

        timeElapsed = to_simple_string(elapsed);
        endTime = to_simple_string(finish);
        timeLeft = to_simple_string(left);
    } else { timeElapsed = "Unknown"; endTime = "Unknown"; timeLeft = "Unknown"; } 

    int rowtop = row;
    move(row,0); clrtoeol();
    printw("+--------------------------------------------------------+");
    move(++row,0); clrtoeol();
    if (searchMode) printw("| Searching for potential networks. %d found thus far.", found);
    else printw("| Running simulations on %d potential networks.", found);

    move(++row,0); clrtoeol();
    printw("| %d/%d (%.3f\%)", total-done, total, percent*100);
    move(++row,0); clrtoeol();
    printw("| Elapsed: %s", timeElapsed.c_str());
    move(++row,0); clrtoeol();
    printw("| Remaining: %s", timeLeft.c_str());
    move(++row,0); clrtoeol();
    printw("| Done At: %s", endTime.c_str());
    
    // Draw progress bar for fun
    move(++row,0); clrtoeol();
    int pb_width = 52;
    int pb_done = pb_width * percent; 
    printw("| >");
    for (int i = 0; i < pb_done ; ++i)
    {
        printw("#");
    }
    for (int i = pb_done; i < pb_width; ++i)
    {
        printw(" ");
    }
    printw("<");
    move(++row,0); clrtoeol();
    printw("+--------------------------------------------------------+");
    for (int i = rowtop+1; i < row; ++i)
    {
        move(i, 57);
        printw("|");
    }
    move(++row,0);
    refresh();
}



int main(int argc, char* argv[]) 
{


    /**********************************************************************/
    /* SETUP THE PROGRAM */
    /**********************************************************************/
    // Determine the program options and set default values
    int nThreads = 4;
    int repeats = 1;
    std::string logFile = "log";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("threads,t", po::value<int>(), "set number of concurrent events")
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
    std::clog << std::setiosflags(std::ios::fixed) << std::setprecision(2);

    printw("Logging results in %s\n", logFile.c_str());
    printw("Initializing simulations...\n");
    refresh();
    boost::threadpool::thread_pool<> threads(nThreads);

    // Define iterators for use later
    std::vector<Simulation>::iterator simit;
    std::map< double, std::vector<Simulation> >::iterator simsimit;
    std::vector< std::vector< std::vector<Simulation> > >::iterator simsimsimit;
    std::map< double, std::vector<Simulation> > durations;
    std::map<int,std::map<double,std::vector<Simulation> > > simbundle;
    std::map< int, std::map< double, std::vector<Simulation> > >::iterator bundleit;
    std::vector<Simulation> sims;


    boost::mt19937 rng;
    boost::normal_distribution<> nd(0.0, 1.0);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > var_nor(rng, nd);
    
    // Seed with current time
    unsigned int RANDOM_SEED = static_cast<unsigned int>(std::time(0));
    rng.seed(RANDOM_SEED);

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

    for (double i = 1.0; i <= 10; i+=1)
    {
        param.push_back(i);
    }
    params.add("tauOn", param);
    params.add("tauOff", param);
    param.clear();
    for (double i = 1; i <= 10; i+=1)
    {
        param.push_back(i);
    }
    params.add("dOn", param);
    params.add("dIOn", param);
    //params.add("dOff", param);
    param.clear();
    param.push_back(0);
    param.push_back(2);
    param.push_back(4);
    param.push_back(8);
    param.push_back(10);
    params.add("gMaxI", param);
    param.push_back(11);
    param.push_back(12);
    param.push_back(14);
    param.push_back(16);
    param.push_back(18);
    params.add("gMaxOn", param);
    params.add("gMaxOff", param);
    param.clear();


    double sS, sE, sI;
    sS = 1;
    sE = 25;
    sI = 1;
    Tuning bp(sS,sE,sI);

    /* Bat Bandpass */
    bp.define(1,0,0);
    bp.define(6,2,1.1);
    bp.define(13,0,0);

    /* Rat Bandpass */
    /*
    bp.define(50,2,1);
    bp.define(5,0,0.5);
    bp.define(100,0,0.5);
    */
    bp.smooth();
    std::clog << "Random Seed: " << RANDOM_SEED << std::endl;
    std::clog << "Spreadsheet results below" << std::endl;
    std::clog << "=========================" << std::endl;
    std::clog << "Score,gMaxOn,gMaxOff,gMaxSus,tauOn,tauOff,dOn,dOff,dIOn";
    for (double i = sS; i <= sE; i+=sI) 
    {
        std::clog << ","<<i;
    }
    std::clog << std::endl;
    std::clog << "1,,,,,,,," << bp.print() << std::endl;

    long numSims; 
    long done;
    ptime start;
    std::map<double,double> trialResults;
    std::map<double,double>::iterator it_trialResults;

    int networkID = 0;
    std::vector<int> goodTrials; // Any searches that yield results store here

    printw("Running simulations...\n");
    refresh();

    int row,col;
    getyx(stdscr,row,col);

    bool searchMode;
    bool goodNetwork = false;
    int actrepeats = 1;
    int numsimsinbundle = 0;

    for (int i_search = 0; i_search < 2; ++i_search)
    {
        searchMode = (i_search == 0);
        if (searchMode) 
        {
            numSims = (bp.numsearchedfor()) * params.size();
        }
        if (!searchMode) 
        {
            numSims = repeats * ((sE-sS)/sI+1) * goodTrials.size();
            actrepeats = repeats;
        }
        start = second_clock::local_time();
        networkID = 0;
        done = 0;
        params.reset();
        for (; !params.done(); params++)
        {
            gMaxOn = params.val("gMaxOn");
            gMaxOff = params.val("gMaxOff");;
            gMaxS = params.val("gMaxI");;
            tOn1 = tOn2 = params.val("tauOn");
            tOff1 = tOff2 = params.val("tauOff");
            dOn = params.val("dOn");

            durations.clear();
            goodNetwork = searchMode || std::find(goodTrials.begin(), goodTrials.end(), networkID) != goodTrials.end();

            if (goodNetwork)
            {
                for (int i = sS; i <= sE; i+=sI)
                {
                    sims.clear();
                    for (int repeat = 0; repeat < actrepeats; ++repeat)
                    {
                        if (!searchMode || bp.useinsearch(i))
                        {
                            Simulation sim;
                            sim.C = 180;
                            sim.useVoltage = false;
                            sim.defaultparams();
                            sim.T = 333;
                            if (searchMode) sim.dt = 0.25;
                            else sim.dt = 0.05;

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
                            SusI.del = dIOn;
                            SusI.tau1 = tS1;
                            SusI.tau2 = tS2;
                            SusI.gMax = gMaxS;

                            if (!searchMode)
                            {
                                OnE.spikes.push_back(0+var_nor());
                                OffE.spikes.push_back(i+var_nor());
                            } else {
                                OnE.spikes.push_back(0);
                                OffE.spikes.push_back(i);
                            }
                            for (int j = 0; j<=+i; ++j)
                            {
                                SusI.spikes.push_back(j);
                            }
                            sim.synapses.push_back(OnE);
                            sim.synapses.push_back(OffE);
                            sim.synapses.push_back(SusI);
                            sims.push_back(sim);
                            numsimsinbundle++;
                            ++done;
                        }
                    }
                    if (!searchMode || bp.useinsearch(i))
                    {
                        durations[i] = sims;
                    }
                }
                simbundle[networkID] = durations;
            }
            ++networkID;
            threads.schedule(boost::bind(&progress, start, done, numSims, goodTrials.size(), searchMode,row));

            if (params.done() || numsimsinbundle > nThreads * 4)
            {
                numsimsinbundle = 0;
                for (bundleit = simbundle.begin(); bundleit != simbundle.end(); bundleit++)
                {
                    for (simsimit = bundleit->second.begin(); simsimit != bundleit->second.end(); simsimit++)
                    {
                        for (simit = simsimit->second.begin(); simit != simsimit->second.end(); simit++)
                        {
                            threads.schedule(boost::bind(&Simulation::runSim,&(*simit)));
                        }
                    }
                }
                threads.wait();

                // Extract results
                double spikes;
                if (goodNetwork)
                {
                    for (bundleit = simbundle.begin(); bundleit != simbundle.end(); bundleit++)
                    {
                        trialResults.clear();
                        for (simsimit = bundleit->second.begin(); simsimit != bundleit->second.end(); simsimit++)
                        {
                            spikes = 0;
                            for (simit = simsimit->second.begin(); simit < simsimit->second.end(); simit++)
                            {
                                spikes += simit->spikes().size();
                            }
                            spikes /= repeats;
                            trialResults[simsimit->first] = spikes;
                        }

                        if (searchMode)
                        {
                            if (bp.possiblematch(trialResults)) {
                                goodTrials.push_back(bundleit->first);
                            }
                        }
                        else
                        {
                            double score = bp.score(trialResults);
                            std::clog << score << "," << gMaxOn << "," <<  gMaxOff << "," <<  gMaxS << "," << tOn1 << "," << tOff1 << "," << dOn << "," << dOff << "," << dIOn ;

                            // Print it out
                            for (it_trialResults = trialResults.begin(); it_trialResults != trialResults.end(); it_trialResults++)
                            {
                                std::clog << "," << it_trialResults->second;
                            }
                            std::clog << std::endl;
                            std::clog << std::flush;
                        }
                    }
                }
                simbundle.clear();
            }
        }

    }

    endwin();

    return 0;
}
