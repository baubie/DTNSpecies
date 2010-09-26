#include "SandboxGTK.h"


SandboxGTK::SandboxGTK(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Window(cobject),
  m_refGlade(refGlade),
  m_pBtnQuit(0),
  m_pBtnRefresh(0)
{
    m_refGlade->get_widget("btnQuit", m_pBtnQuit);
    if (m_pBtnQuit)
        m_pBtnQuit->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnQuit_clicked));
        
    m_refGlade->get_widget("btnRefresh", m_pBtnRefresh);
    if (m_pBtnRefresh)
        m_pBtnRefresh->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnRefresh_clicked));

    m_refGlade->get_widget("hboxMain", m_pHboxMain);
    if (m_pHboxMain)
    {
        m_pPlot = new PlotMM::Plot();
        m_pPlot->scale(PlotMM::AXIS_RIGHT)->set_enabled(false);
        m_pPlot->scale(PlotMM::AXIS_TOP)->set_enabled(false);
        m_pHboxMain->pack_end(*m_pPlot, Gtk::PACK_EXPAND_WIDGET, 5);
    }

}




SandboxGTK::~SandboxGTK()
{
}


void SandboxGTK::on_btnQuit_clicked()
{
    delete m_pPlot;
    hide();
}

void SandboxGTK::on_btnRefresh_clicked()
{
    Simulation sim;
    sim.defaultparams();
    sim.useVoltage = true;
    sim.C = 120;
    sim.T = 250;
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
    }


    Glib::RefPtr<PlotMM::Curve> voltageCurve(new PlotMM::Curve("Voltage"));
    voltageCurve->set_data(ct,cV,V.size());
    m_pPlot->scale(PlotMM::AXIS_LEFT)->set_range(-75,50,false);
    m_pPlot->add_curve(voltageCurve);
}

