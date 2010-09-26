
#include "SandboxGTK.h"


class SandboxGTK 
{
    public:
        int run(int argc, char **argv);
        void on_btnQuit_clicked()
        {
            if (this->m_pMainWindow)
            {
                delete this->m_pPlot;
                this->m_pMainWindow->hide();
            }
        }

        void on_btnRefresh_clicked()
        {
            Simulation sim;
            sim.defaultparams();
            sim.useVoltage = true;
            sim.C = 120;
            sim.T = 100;
            sim.dt = 0.01;

            Synapse AMPA;
            AMPA.gMax = 25; // nS
            AMPA.tau1 = 0.8; // ms
            AMPA.tau2 = 3; // ms
            AMPA.del = 25; // ms
            AMPA.E = 0;
            AMPA.spikes.push_back(0);

            Synapse::dt = sim.dt;
            Synapse::prepare(0.8,3);

            sim.synapses.push_back(AMPA);
            sim.runSim();
            std::vector<double> V = sim.voltagetrace();
            std::vector<double> t = sim.timesteps();
            double cV[V.size()],ct[t.size()];
            for (unsigned int i = 0; i < V.size(); ++i) 
            {
                cV[i] = V[i];
                ct[i] = t[i];
                if (i < 300)
                std::cout << cV[i] << std::endl;
            }


            Glib::RefPtr<PlotMM::Curve> voltageCurve(new PlotMM::Curve("Voltage"));
            voltageCurve->set_data(ct,cV,V.size());
            this->m_pPlot->scale(PlotMM::AXIS_LEFT)->set_range(-75,50,false);
            this->m_pPlot->add_curve(voltageCurve);
        }

    private:
        Gtk::Window* m_pMainWindow;
        PlotMM::Plot* m_pPlot;
};


int SandboxGTK::run(int argc, char **argv)
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
    refBuilder->get_widget("MainWindow", this->m_pMainWindow);
    if (this->m_pMainWindow)
    {
        Gtk::ToolButton* pBtnQuit = 0;
        refBuilder->get_widget("btnQuit", pBtnQuit);
        if (pBtnQuit)
            pBtnQuit->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnQuit_clicked));


        Gtk::ToolButton* pBtnRefresh = 0;
        refBuilder->get_widget("btnRefresh", pBtnRefresh);
        if (pBtnRefresh)
            pBtnRefresh->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnRefresh_clicked));

        Gtk::HBox* pHboxMain = 0;
        refBuilder->get_widget("hboxMain", pHboxMain);
        if (pHboxMain)
        {
            this->m_pPlot = new PlotMM::Plot();
            this->m_pPlot->scale(PlotMM::AXIS_RIGHT)->set_enabled(false);
            this->m_pPlot->scale(PlotMM::AXIS_TOP)->set_enabled(false);
            pHboxMain->pack_end(*this->m_pPlot, Gtk::PACK_EXPAND_WIDGET, 5);
        }
    }
    Gtk::Main::run(*this->m_pMainWindow);
    return 0;
}

