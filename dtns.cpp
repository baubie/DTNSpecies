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

void progress(ptime start, long done, long total, int found, bool searchMode, int row, bool force);


int main(int argc, char* argv[]) 
{

    /**********************************************************************/
    /* Define Program Parameters                                          */
    /**********************************************************************/
    double sS, sE, sI;
    vectors<double> params;
    std::vector<double> param;
    double simC, simT;

    // Setup stimulus and basic neuron properties

    double tOn1,tOn2,tOff1,tOff2,tS1,tS2,dOn,dOff,dIOn,gMaxOn,gMaxOff,gMaxS;
    /* Mouse Bandpass */
    /*
    sS = 1; sE = 100; sI = 1;
    simC = 200; simT = 333;
    tS1 = 0.1; tS2 = 2;
    Tuning bp(sS,sE,sI);
    bp.define(5,0,0);
    bp.define(40,1,1.1);
    bp.define(80,0,0);
    bp.smooth();
    for (double i = 2.0; i <= 20; i+=2) param.push_back(i);
    params.add("tauOn", param);
    params.add("tauOff", param);
    param.clear();
    for (double i = 0; i <= 80; i+=5) param.push_back(i);
    params.add("dOn", param);
    param.clear();
    param.push_back(5);
    params.add("dOff", param);
    param.clear();
    param.push_back(0);
    params.add("dIOn", param);
    param.clear();
    for (double i = 0; i <= 8; i+=2) param.push_back(i);
    params.add("gMaxI", param);
    for (double i = 10; i <= 20; i+=2) param.push_back(i);
    params.add("gMaxOn", param);
    params.add("gMaxOff", param);
    param.clear();
    */

    /* Rat Bandpass */
    /*
    sS = 5; sE = 200; sI = 5;
    simC = 200; simT = 500;
    tS1 = 0.1; tS2 = 2;
    Tuning bp(sS,sE,sI);
    bp.define(5,0,0);
    bp.define(80,1,1.1);
    bp.define(150,0,0);
    bp.smooth();
    for (double i = 2.0; i <= 20; i+=2) param.push_back(i);
    params.add("tauOn", param);
    params.add("tauOff", param);
    param.clear();
    for (double i = 0; i <= 120; i+=5) param.push_back(i);
    params.add("dOn", param);
    param.clear();
    param.push_back(5);
    params.add("dOff", param);
    param.clear();
    param.push_back(0);
    params.add("dIOn", param);
    param.clear();
    for (double i = 0; i <= 8; i+=2) param.push_back(i);
    params.add("gMaxI", param);
    for (double i = 10; i <= 20; i+=2) param.push_back(i);
    params.add("gMaxOn", param);
    params.add("gMaxOff", param);
    param.clear();
    */

    /* Bat Bandpass */
    /*
    sS = 1; sE = 30; sI = 1;
    simC = 180; simT = 333;
    tS1 = 0.1; tS2 = 2;
    Tuning bp(sS,sE,sI);
    bp.define(1,0,0);
    bp.define(6,2,1.1);
    bp.define(13,0,0);
    bp.smooth();
    for (double i = 1.0; i <= 8; i+=1) param.push_back(i);
    params.add("tauOn", param);
    params.add("tauOff", param);
    param.clear();
    for (double i = 1; i <= 20; i+=1) param.push_back(i);
    params.add("dOn", param);
    param.clear();
    param.push_back(5);
    params.add("dOff", param);
    param.clear();
    param.push_back(0);
    params.add("dIOn", param);
    param.clear();
    for (double i = 0; i <= 4; i+=2) param.push_back(i);
    params.add("gMaxI", param);
    for (double i = 6; i <= 16; i+=2) param.push_back(i);
    params.add("gMaxOn", param);
    params.add("gMaxOff", param);
    param.clear();
    */


    /* Bat Shortpass */
    sS = 1; sE = 20; sI = 1;
    simC = 180; simT = 50;
    tS1 = 0.1; tS2 = 2;
    Tuning bp(sS,sE,sI);

    bp.define(1,2,0);
    bp.reldefine(5,Tuning::LTEQ,1);
    bp.define(15,0,1.0);

    bp.smooth();
    for (double i = 1.0; i <= 8; i+=1) param.push_back(i);
    params.add("tauOn", param);
    params.add("tauOff", param);
    param.clear();
    for (double i = 1; i <= 20; i+=1) param.push_back(i);
    params.add("dOn", param);
    params.add("dIOn", param);
    param.clear();
    param.push_back(5);
    params.add("dOff", param);
    param.clear();
    for (double i = 0; i <= 4; i+=2) param.push_back(i);
    params.add("gMaxI", param);
    for (double i = 6; i <= 16; i+=2) param.push_back(i);
    params.add("gMaxOn", param);
    params.add("gMaxOff", param);
    param.clear();

    /**********************************************************************/
    /* Read CLI Parameters                                                */
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
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLUE, -1);

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







    Simulation defs;
    defs.defaultparams();
    std::clog << "Duration Tune Neuron Network Searcher" << std::endl;
    std::clog << "Parameters Searched:" << std::endl;
    std::clog << params.toString();
    std::clog << "Trials per network: " << repeats << std::endl;
    std::clog << "Random Seed: " << RANDOM_SEED << std::endl;
    std::clog << "Time per trial: " << simT << " ms" << std::endl;
    std::clog << "Start Time: " << sS << std::endl;
    std::clog << "End Time: " << sE << std::endl;
    std::clog << "Interval Time: " << sI << std::endl;
    std::clog << "C: " << simC;
    std::clog << "\tgL: " << defs.gL;
    std::clog << "\tEL: " << defs.EL;
    std::clog << "\ta: " << defs.a;
    std::clog << "\tb: " << defs.b;
    std::clog << "\tdT: " << defs.dT;
    std::clog << "\tVT: " << defs.VT;
	std::clog << "\tVr: " << defs.Vr;
    std::clog << "\ttau_w: " << defs.tau_w;
    std::clog << std::endl;
    std::clog << "Spreadsheet results below" << std::endl;
    std::clog << "=========================" << std::endl;
    std::clog << "gMaxOn,gMaxOff,gMaxSus,tauOn,tauOff,dOn,dOff,dIOn";
    for (double i = sS; i <= sE; i+=sI) 
    {
        std::clog << ","<<i;
    }
    std::clog << std::endl;
    std::clog << ",,,,,,," << bp.print() << std::endl;

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
    int simsperbundle;
    int numNetworks = 0;

    for (int i_search = 0; i_search < 2; ++i_search)
    {
        searchMode = (i_search == 0);

        if (searchMode) 
        {
            numSims = (bp.numsearchedfor()) * params.size();
            numNetworks = params.size();
            simsperbundle = nThreads * 50;
        }
        if (!searchMode) 
        {
            numSims = repeats * ((sE-sS)/sI+1) * goodTrials.size();
            numNetworks = goodTrials.size();
            actrepeats = repeats;
            simsperbundle = nThreads * 50;
        }
        start = second_clock::local_time();
        networkID = 0;
        done = 0;
        numsimsinbundle = 0;
        params.reset();
        progress(start,done*(numNetworks/(1.0*numSims)),numNetworks,goodTrials.size(),searchMode,row,true);

        for (; !params.done(); params++)
        {
            goodNetwork = searchMode || std::find(goodTrials.begin(), goodTrials.end(), networkID) != goodTrials.end();
            if (goodNetwork)
            {
                durations.clear();
                gMaxOn = params.val("gMaxOn");
                gMaxOff = params.val("gMaxOff");;
                gMaxS = params.val("gMaxI");;
                tOn1 = tOn2 = params.val("tauOn");
                tOff1 = tOff2 = params.val("tauOff");
                dOn = params.val("dOn");
                dOff = params.val("dOff");
                dIOn = params.val("dIOn");
                for (int i = sS; i <= sE; i+=sI)
                {
                    sims.clear();
                    for (int repeat = 0; repeat < actrepeats; ++repeat)
                    {
                        if (!searchMode || bp.useinsearch(i))
                        {
                            Simulation sim;
                            sim.C = simC;
                            sim.useVoltage = false;
                            sim.defaultparams();
                            sim.T = simT; 
                            if (searchMode) sim.dt = 0.25;
                            else sim.dt = 0.10;

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

                            if (!searchMode && actrepeats > 1)
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
                        }
                    }
                    if (!searchMode || bp.useinsearch(i))
                    {
                        durations[i] = sims;
                    }
                }
                simbundle[networkID] = durations;

                if (params.size() == networkID+1 || numsimsinbundle > simsperbundle)
                {
                    numsimsinbundle = 0;
                    // Schedule all of the simulations to run
                    int count = 0;
                    for (bundleit = simbundle.begin(); bundleit != simbundle.end(); bundleit++)
                    {
                        for (simsimit = bundleit->second.begin(); simsimit != bundleit->second.end(); simsimit++)
                        {
                            for (simit = simsimit->second.begin(); simit != simsimit->second.end(); simit++)
                            {
                                threads.schedule(boost::bind(&Simulation::runSim,&(*simit)));
                                ++count;
                                ++done;
                                if (count >= simsperbundle) {
                                    progress(start,done*(numNetworks/(1.0*numSims)),numNetworks,goodTrials.size(),searchMode,row,false);
                                    threads.wait();
                                    count = 0;
                                }
                            }
                        }
                    }
                    threads.wait();

                    // Extract results
                    double spikes;
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
                            spikes /= actrepeats;
                            trialResults[simsimit->first] = spikes;
                        }

                        if (searchMode)
                        {
                            if (bp.possiblematch(trialResults)) {
                                // If it is a possible match, push the networkID onto the goodTrials vector
                                goodTrials.push_back(bundleit->first);
                            }
                        }
                        else if (goodNetwork)
                        {
                            int netID = bundleit->first;
                            std::clog << std::setiosflags(std::ios::fixed) << std::setprecision(2);
                            std::clog << params.val("gMaxOn", netID) << "," <<  params.val("gMaxOff", netID) << "," <<  params.val("gMaxI", netID) << ",";
                            std::clog << params.val("tauOn",netID) << "," << params.val("tauOff",netID) << "," << params.val("dOn",netID) << "," << params.val("dOff",netID) << "," << dIOn ;
                            // Print it out
                            for (it_trialResults = trialResults.begin(); it_trialResults != trialResults.end(); it_trialResults++)
                            {
                                std::clog << "," << it_trialResults->second;
                            }
                            std::clog << std::endl;
                            std::clog << std::flush;
                        }
                    }
                    simbundle.clear();
                }
            }
            ++networkID;
        }
    }
    progress(start,done*(numNetworks/(1.0*numSims)),numNetworks,goodTrials.size(),searchMode,row,true);
    printw("\nNetwork search completed.\n");
    endwin();

    return 0;
}



void progress(ptime start, long done, long total, int found, bool searchMode, int row, bool force)
{
    // Update at most twice a second for sanity sake
    static ptime last;
    static char spin[] = "/-\\|/-\\|";
    static int spinpos = 0;

    if (!force && (microsec_clock::local_time() - last).total_microseconds() < 100000) return;
    last = microsec_clock::local_time();

    spinpos++;
    if (spinpos == 8) spinpos = 0;

    std::string endTime, timeLeft, timeElapsed;
    double percent = ((double)done/(double)(total+0.000001));
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
    printw("+--------------------------------------------------------+");
    move(++row,0); clrtoeol();
    printw("| [");
    attron(COLOR_PAIR(1));
    attron(A_BOLD);
    printw("%c", spin[spinpos]);
    attroff(COLOR_PAIR(1));
    attroff(A_BOLD);
    printw("] %d/%d (%.3f\%)", total-done, total, percent*100);
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
        if (i != rowtop+2)
        {
            move(i, 57);
            printw("|");
        }
    }
    move(++row,0);
    refresh();
}

