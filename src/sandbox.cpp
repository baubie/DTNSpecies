#include <gtkmm.h>
#include <iostream>
#include <vector>

#include "synapse.h"
#include "simulation.h"

Gtk::Window* pMainWindow = 0;

static void on_btnQuit_clicked()
{
    if (pMainWindow)
        pMainWindow->hide();
}

static void on_btnRefresh_clicked()
{
    Simulation sim;
    sim.useVoltage = true;
    sim.defaultparams();

    Synapse AMPA;
    AMPA.gMax = 10;
    AMPA.tau1 = 0.8;
    AMPA.tau2 = 3;
    AMPA.del = 0;
    AMPA.spikes.push_back(0);

    sim.synapses.push_back(AMPA);
    sim.runSim();

    std::vector<double> V = sim.voltagetrace();
}

int main(int argc, char **argv) 
{
    Gtk::Main kit(argc, argv);

    // Load the GtkBuilder file and instantiate its widgets
    Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
    try
    {
        refBuilder->add_from_file("sandbox.glade");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }

    //Get the main window
    refBuilder->get_widget("MainWindow", pMainWindow);
    if (pMainWindow)
    {
        Gtk::ToolButton* pBtnQuit = 0;
        refBuilder->get_widget("btnQuit", pBtnQuit);
        if (pBtnQuit)
            pBtnQuit->signal_clicked().connect( sigc::ptr_fun(on_btnQuit_clicked) );


        Gtk::ToolButton* pBtnRefresh = 0;
        refBuilder->get_widget("btnRefresh", pBtnRefresh);
        if (pBtnRefresh)
            pBtnRefresh->signal_clicked().connect( sigc::ptr_fun(on_btnRefresh_clicked) );

        kit.run(*pMainWindow);
    }

    return 0;
}
