#include <gtkmm.h>

#include <iostream>
#include <vector>

#include <plotmm/plot.h>
#include <plotmm/scalediv.h>
#include <plotmm/errorcurve.h>
#include <plotmm/symbol.h>
#include <plotmm/paint.h>

#include "synapse.h"
#include "simulation.h"

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
