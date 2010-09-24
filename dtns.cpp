#include <iostream>
#include <fstream>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "boost/threadpool.hpp"
#include <boost/math/distributions/normal.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>
#include <math.h>
#include <ncurses.h>

#include "simulation.h"
#include "synapse.h"
#include "tuning.h"
#include "vectors.h"

namespace po = boost::program_options;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;


void progress(ptime start, long done, long total, int found, bool searchMode, int row, bool force);
vector<double> makeDurations();

int main(int argc, char* argv[]) 
{

    /**********************************************************************/
    /* Define Program Parameters                                          */
    /**********************************************************************/
    //double durs[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,25,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200};
    double durs[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,25};
    vector<double> stimuli(durs, durs+sizeof(durs)/sizeof(double));
    vector<double>::iterator stim_it;


    vectors<double> params;
    double simC, simT;
    simC = 180; simT = 50;

    Tuning bp;
    bp.define("Shortpass1", 1, -1, 3);
    //bp.define("Shortpass2", 1, -1, 5);
    //bp.define("Shortpass3", 1, -1, 10);
    bp.check(10);


    vector<double> param;
    for (double i = 1.0; i <= 4; i+=1) param.push_back(i);
    params.add("tauOn", param);
    params.add("tauOff", param);
    param.clear();
    for (double i = 1; i <= 10; i+=1) param.push_back(i);
    params.add("dOn", param);
    param.clear();
    param.push_back(5);
    params.add("dOff", param);
    param.clear();
    param.push_back(1);
    params.add("dSus", param);
    param.clear();
    for (double i = 0; i <= 4; i+=2) param.push_back(i);
    params.add("gMaxS", param);
    for (double i = 6; i <= 16; i+=2) param.push_back(i);
    params.add("gMaxOn", param);
    params.add("gMaxOff", param);
    param.clear();
    param.push_back(2);
    params.add("tauS", param);
    param.clear();



    double jitter[] = {-0.3,-0.1,0,0.1,0.3};
    int repeats = sizeof(jitter)/sizeof(double);



    /**********************************************************************/
    /* Read CLI Parameters                                                */
    /**********************************************************************/
    // Determine the program options and set default values
    int nThreads = 4;
    string logFile = "log";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("threads,t", po::value<int>(), "set number of concurrent events")
        ("logfile,l", po::value<string>(), "log file")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if(vm.count("help")) {
        cout << desc << endl;
        return 1;
    }
    if(vm.count("threads")) {
        nThreads = vm["threads"].as<int>();
    }
    if(vm.count("logfile")) {
        logFile = vm["logfile"].as<string>();
    }
    /**********************************************************************/


    // Setup NCURSES
    initscr();
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLUE, -1);

    printw("Network Iterator - Version 1.0\n");
    refresh();

    ofstream oflog(logFile.c_str());
    clog.rdbuf(oflog.rdbuf());
    clog << setiosflags(ios::fixed) << setprecision(2);

    printw("Logging results in %s\n", logFile.c_str());
    printw("Initializing simulations...\n");
    refresh();
    boost::threadpool::thread_pool<> threads(nThreads);

    // Define iterators for use later
    vector<Simulation>::iterator simit;
    map< double, vector<Simulation> >::iterator simsimit;
    vector< vector< vector<Simulation> > >::iterator simsimsimit;
    map< double, vector<Simulation> > durations;
    map<int,map<double,vector<Simulation> > > simbundle;
    map< int, map< double, vector<Simulation> > >::iterator bundleit;
    vector<Simulation> sims;







    Simulation defs;
    defs.defaultparams();
    clog << "Duration Tune Neuron Network Searcher" << endl;
    clog << "Parameters Searched:" << endl;
    clog << params.toString();
    clog << "Time per trial: " << simT << " ms" << endl;
    clog << "C: " << simC;
    clog << "\tgL: " << defs.gL;
    clog << "\tEL: " << defs.EL;
    clog << "\ta: " << defs.a;
    clog << "\tb: " << defs.b;
    clog << "\tdT: " << defs.dT;
    clog << "\tVT: " << defs.VT;
	clog << "\tVr: " << defs.Vr;
    clog << "\ttau_w: " << defs.tau_w;
    clog << endl;
    clog << "Spreadsheet results below" << endl;
    clog << "=========================" << endl;
    clog << "gMaxOn,gMaxOff,gMaxSus,tauOn,tauOff,dOn,dOff,dSus";
    for (unsigned int i = 0; i < stimuli.size(); ++i) 
    {
        clog << ","<< stimuli[i];
    }
    clog << endl;

    long done;
    ptime start;
    map<double,double> trialResults;
    map<double,double>::iterator it_trialResults;

    int networkID = 0;
    vector<int> goodTrials; // Any searches that yield results store here

    printw("Running simulations...\n");
    refresh();

    int row,col;
    getyx(stdscr,row,col);

    bool searchMode;
    bool goodNetwork = false;
    int numsimsinbundle = 0;
    int simsperbundle;
    int numNetworks = 0;

    for (int i_search = 0; i_search < 2; ++i_search)
    {
        searchMode = (i_search == 0);

        if (searchMode) 
        {
            numNetworks = params.size();
            simsperbundle = nThreads * 50;
        }
        if (!searchMode) 
        {
            numNetworks = goodTrials.size();
            simsperbundle = nThreads * 50;
        }
        start = second_clock::local_time();
        networkID = 0;
        done = 0;
        numsimsinbundle = 0;
        params.reset();
        progress(start,done,numNetworks,goodTrials.size(),searchMode,row,true);

        for (; !params.done(); params++)
        {
            ++done;
            goodNetwork = searchMode || find(goodTrials.begin(), goodTrials.end(), networkID) != goodTrials.end();
            if (goodNetwork)
            {
                durations.clear();
                for (stim_it = stimuli.begin(); stim_it != stimuli.end(); stim_it++)
                {
                    sims.clear();
                    for (int repeat = 0; repeat < repeats; ++repeat)
                    {
                        if (!searchMode || bp.useinsearch(*stim_it))
                        {
                            Simulation sim;
                            sim.C = simC;
                            sim.useVoltage = false;
                            sim.defaultparams();
                            sim.T = simT; 
                            if (searchMode) sim.dt = 0.10;
                            else sim.dt = 0.10;

                            Synapse OnE;
                            Synapse OffE;
                            Synapse SusI;
                            OnE.gMax = params.val("gMaxOn");
                            OnE.tau1 = params.val("tauOn");
                            OnE.tau2 = params.val("tauOn");
                            OnE.del = params.val("dOn");
                            OffE.gMax = params.val("gMaxOff");
                            OffE.tau1 = params.val("tauOff");
                            OffE.tau2 = params.val("tauOff");
                            OffE.del = params.val("dOff");
                            SusI.E = -75;
                            SusI.del = params.val("dSus");
                            SusI.tau1 = params.val("tauS");
                            SusI.tau2 = params.val("tauS");
                            SusI.gMax = params.val("gMaxS");

                            OnE.spikes.push_back(0+jitter[repeat]);
                            OffE.spikes.push_back(*stim_it);
                            for (int j = 0; j<=+*stim_it; ++j)
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
                    if (!searchMode || bp.useinsearch(*stim_it))
                    {
                        durations[*stim_it] = sims;
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
                                if (count >= simsperbundle) {
                                    progress(start,done,numNetworks,goodTrials.size(),searchMode,row,false);
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
                            spikes /= repeats;
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
                            clog << setiosflags(ios::fixed) << setprecision(2);
                            clog << params.val("gMaxOn", netID) << "," <<  params.val("gMaxOff", netID) << "," <<  params.val("gMaxS", netID) << ",";
                            clog << params.val("tauOn",netID) << "," << params.val("tauOff",netID) << "," << params.val("dOn",netID) << "," << params.val("dOff",netID) << "," << params.val("dSus") ;
                            // Print it out
                            for (it_trialResults = trialResults.begin(); it_trialResults != trialResults.end(); it_trialResults++)
                            {
                                clog << "," << it_trialResults->second;
                            }
                            clog << endl;
                            clog << flush;
                        }
                    }
                    simbundle.clear();
                }
            }
            ++networkID;
        }
    }
    progress(start,done,numNetworks,goodTrials.size(),searchMode,row,true);
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

    string endTime, timeLeft, timeElapsed;
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
