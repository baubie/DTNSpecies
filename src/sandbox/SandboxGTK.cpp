#include "SandboxGTK.h"


SandboxGTK::SandboxGTK(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Window(cobject),
  m_refGlade(refGlade),
  m_pBtnQuit(0),
  m_pBtnRefresh(0)
{

    set_title("Duration-Tuned Neuron Sandbox (Version 0.3)");

    // Setup the toolbar
    m_refGlade->get_widget("btnQuit", m_pBtnQuit);
    if (m_pBtnQuit)
        m_pBtnQuit->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnQuit_clicked));
        
    m_refGlade->get_widget("btnRunStimuli", m_pBtnRefresh);
    if (m_pBtnRefresh)
        m_pBtnRefresh->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnRunStimuli_clicked));

    // Setup the Main Window
    m_refGlade->get_widget("hboxMain", m_pHBoxMain);
    if (m_pHBoxMain)
    {

        // Create network parameters TreeView
        m_refNetworkTree = Gtk::TreeStore::create(m_NetworkColumns);
        m_NetworkList.set_model(m_refNetworkTree);
        m_NetworkList.append_column("Name", m_NetworkColumns.m_col_name);

        // Special control over the value column
        m_treeviewcolumn_value.set_title("Value");
        m_treeviewcolumn_value.pack_start(m_renderer_value);
        m_NetworkList.append_column(m_treeviewcolumn_value);
        m_treeviewcolumn_value.set_cell_data_func(m_renderer_value,
            sigc::mem_fun(*this, &SandboxGTK::networktree_value_cell_data) );
        m_renderer_value.signal_edited().connect(
            sigc::mem_fun(*this, &SandboxGTK::networktree_value_on_edited) );



        // Add default options to tree view
        Simulation sim;
        sim.defaultparams();
        sim.T = 150;
        sim.dt = 0.01;
        Gtk::TreeModel::Row row;
        Gtk::TreeModel::Row childrow;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "Simulation";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "T";
        childrow[m_NetworkColumns.m_col_value] = sim.T;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "dt";
        childrow[m_NetworkColumns.m_col_value] = sim.dt;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "delay";
        childrow[m_NetworkColumns.m_col_value] = sim.del;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "DTN";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "C";
        childrow[m_NetworkColumns.m_col_value] = sim.C;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "gL";
        childrow[m_NetworkColumns.m_col_value] = sim.gL;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "EL";
        childrow[m_NetworkColumns.m_col_value] = sim.EL;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "VT";
        childrow[m_NetworkColumns.m_col_value] = sim.VT;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "dT";
        childrow[m_NetworkColumns.m_col_value] = sim.dT;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "a";
        childrow[m_NetworkColumns.m_col_value] = sim.a;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "tauw";
        childrow[m_NetworkColumns.m_col_value] = sim.tau_w;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "b";
        childrow[m_NetworkColumns.m_col_value] = sim.b;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Vr";
        childrow[m_NetworkColumns.m_col_value] = sim.Vr;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "OnsetGlu";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Spikes";
        childrow[m_NetworkColumns.m_col_value] = 1;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Delay";
        childrow[m_NetworkColumns.m_col_value] = 0;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Interval";
        childrow[m_NetworkColumns.m_col_value] = 1;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "OffsetGlu";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Spikes";
        childrow[m_NetworkColumns.m_col_value] = 1;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Delay";
        childrow[m_NetworkColumns.m_col_value] = 0;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Interval";
        childrow[m_NetworkColumns.m_col_value] = 1;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "OnsetGABA";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Spikes";
        childrow[m_NetworkColumns.m_col_value] = -1;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Delay";
        childrow[m_NetworkColumns.m_col_value] = 0;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Interval";
        childrow[m_NetworkColumns.m_col_value] = 1;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "AMPA";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "gMax";
        childrow[m_NetworkColumns.m_col_value] = 12;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "tau1";
        childrow[m_NetworkColumns.m_col_value] = 1;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "tau2";
        childrow[m_NetworkColumns.m_col_value] = 5;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "E";
        childrow[m_NetworkColumns.m_col_value] = 0;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Delay";
        childrow[m_NetworkColumns.m_col_value] = 0;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "NMDA";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "gMax";
        childrow[m_NetworkColumns.m_col_value] = 6;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "tau1";
        childrow[m_NetworkColumns.m_col_value] = 4;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "tau2";
        childrow[m_NetworkColumns.m_col_value] = 20;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "E";
        childrow[m_NetworkColumns.m_col_value] = 0;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Delay";
        childrow[m_NetworkColumns.m_col_value] = 5;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "GABA_A";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "gMax";
        childrow[m_NetworkColumns.m_col_value] = 3;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "tau1";
        childrow[m_NetworkColumns.m_col_value] = 1;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "tau2";
        childrow[m_NetworkColumns.m_col_value] = 5;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "E";
        childrow[m_NetworkColumns.m_col_value] = -75;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Delay";
        childrow[m_NetworkColumns.m_col_value] = 0;


        // Create stimulus TreeView
        m_refStimulusTree = Gtk::ListStore::create(m_StimulusColumns);
        m_StimulusList.set_model(m_refStimulusTree);
        m_StimulusList.append_column("Name", m_StimulusColumns.m_col_name);
        m_StimulusList.append_column_editable("Dur", m_StimulusColumns.m_col_dur);
        m_StimulusList.append_column_editable("Count", m_StimulusColumns.m_col_count);
        m_StimulusList.append_column_editable("Gap", m_StimulusColumns.m_col_gap);


        // Add stimuli
        row = *(m_refStimulusTree->append());
        row[m_StimulusColumns.m_col_name] = "1 ms";
        row[m_StimulusColumns.m_col_dur] = 1;
        row[m_StimulusColumns.m_col_count] = 1;
        row[m_StimulusColumns.m_col_gap] = 0;
        row = *(m_refStimulusTree->append());
        row[m_StimulusColumns.m_col_name] = "5 ms";
        row[m_StimulusColumns.m_col_dur] = 5;
        row[m_StimulusColumns.m_col_count] = 1;
        row[m_StimulusColumns.m_col_gap] = 0;

        row = *(m_refStimulusTree->append());
        row[m_StimulusColumns.m_col_name] = "10 ms";
        row[m_StimulusColumns.m_col_dur] = 10;
        row[m_StimulusColumns.m_col_count] = 1;
        row[m_StimulusColumns.m_col_gap] = 0;

        row = *(m_refStimulusTree->append());
        row[m_StimulusColumns.m_col_name] = "15 ms";
        row[m_StimulusColumns.m_col_dur] = 15;
        row[m_StimulusColumns.m_col_count] = 1;
        row[m_StimulusColumns.m_col_gap] = 0;

        row = *(m_refStimulusTree->append());
        row[m_StimulusColumns.m_col_name] = "25 ms";
        row[m_StimulusColumns.m_col_dur] = 25;
        row[m_StimulusColumns.m_col_count] = 1;
        row[m_StimulusColumns.m_col_gap] = 0;

        row = *(m_refStimulusTree->append());
        row[m_StimulusColumns.m_col_name] = "50 ms";
        row[m_StimulusColumns.m_col_dur] = 50;
        row[m_StimulusColumns.m_col_count] = 1;
        row[m_StimulusColumns.m_col_gap] = 0;

        // Add Fill left VBox 
        m_VBoxLeft.pack_start(m_NetworkList);
        m_VBoxLeft.pack_start(m_StimulusList);
        m_LeftScrollWindow.add(m_VBoxLeft);
        m_LeftScrollWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

        // Add VBoxes to main window
        m_pHBoxMain->pack_start(m_LeftScrollWindow, Gtk::PACK_SHRINK, 0);
        m_pHBoxMain->pack_start(m_VBoxRight, Gtk::PACK_EXPAND_WIDGET, 0);
    }

    show_all_children();

}




SandboxGTK::~SandboxGTK()
{
}

void SandboxGTK::networktree_value_cell_data(Gtk::CellRenderer* /*renderer*/, const Gtk::TreeModel::iterator& iter)
{
    if (iter)
    {
        Gtk::TreeModel::Row row = *iter;

        if (iter->parent() != 0)
        {
            m_renderer_value.property_editable() = true;
            double value = row[m_NetworkColumns.m_col_value];
            char buffer[8];
            sprintf(buffer, "%.3f", value);
            Glib::ustring view_text = buffer;
            m_renderer_value.property_text() = view_text;
        } else {
            m_renderer_value.property_editable() = false;
            m_renderer_value.property_text() = "";
        }
    }
}

void SandboxGTK::networktree_value_on_edited(const Glib::ustring& path_string, const Glib::ustring& new_text)
{
    Gtk::TreePath path(path_string);

    // Convert text to float
    char* pchEnd = 0;
    float new_value = strtod(new_text.c_str(), &pchEnd);
    Gtk::TreeModel::iterator iter = m_refNetworkTree->get_iter(path);
    if (iter)
    {
        Gtk::TreeModel::Row row = *iter;
        row[m_NetworkColumns.m_col_value] = new_value;
    }
}


void SandboxGTK::deletePlots()
{
    while (!m_pPlot.empty())
    {
        delete m_pPlot.back();
        m_pPlot.pop_back();
    }
}

void SandboxGTK::on_btnQuit_clicked()
{
    deletePlots();
    delete m_pBtnQuit;
    delete m_pBtnRefresh;
    delete m_pHBoxMain;
    hide();
}

void SandboxGTK::on_btnRunStimuli_clicked()
{
    runSim();
}

Simulation SandboxGTK::getSimulation()
{
    Simulation sim;
    sim.defaultparams();
    sim.useVoltage = true;
    // Get values from TreeModel

    typedef Gtk::TreeModel::Children type_children;
    type_children children = m_refNetworkTree->children();
    for (type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring parent = row[m_NetworkColumns.m_col_name];
        for (type_children::iterator iter2 = row.children().begin(); iter2 != row.children().end(); ++iter2)
        {
            Gtk::TreeModel::Row childrow = *iter2;
            Glib::ustring child = childrow[m_NetworkColumns.m_col_name];
            double value = childrow[m_NetworkColumns.m_col_value];
            if (parent == "Simulation")
            {
                if(child == "T")
                        sim.T = value;

                if(child == "dt")
                        sim.dt = value;

                if(child == "Delay")
                        sim.del = value;
            }
            if (parent == "DTN")
            {
                if(child == "C")
                        sim.C = value;
                if(child == "gL")
                        sim.gL = value;
                if(child == "EL")
                        sim.EL = value;
                if(child == "VT")
                        sim.VT = value;
                if(child == "dT")
                        sim.dT = value;
                if(child == "a")
                        sim.a = value;
                if(child == "tauw")
                        sim.tau_w = value;
                if(child == "b")
                        sim.b = value;
                if(child == "Vr")
                        sim.VT = value;
            }
        }

    }
    return sim;
}
void SandboxGTK::runSim()
{

    int onset_spikes_glu = 0;
    int offset_spikes_glu = 0;
    int onset_spikes_GABA = 0;
    double onset_delay_glu = 0;
    double offset_delay_glu = 0;
    double onset_delay_GABA = 0;
    double onset_interval_GABA = 0;
    double onset_interval_glu = 0;
    double offset_interval_glu = 0;


    double AMPA_gMax = 0;
    double AMPA_tau1 = 0;
    double AMPA_tau2 = 0;
    double AMPA_E = 0;
    double AMPA_del = 0;

    double NMDA_gMax = 0;
    double NMDA_tau1 = 0;
    double NMDA_tau2 = 0;
    double NMDA_E = 0;
    double NMDA_del = 0;

    double GABA_gMax = 0;
    double GABA_tau1 = 0;
    double GABA_tau2 = 0;
    double GABA_E = 0;
    double GABA_del = 0;

    deletePlots();

    typedef Gtk::TreeModel::Children type_children;
    type_children children = m_refNetworkTree->children();
    for (type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring parent = row[m_NetworkColumns.m_col_name];
        for (type_children::iterator iter2 = row.children().begin(); iter2 != row.children().end(); ++iter2)
        {
            Gtk::TreeModel::Row childrow = *iter2;
            Glib::ustring child = childrow[m_NetworkColumns.m_col_name];
            double value = childrow[m_NetworkColumns.m_col_value];
            if (parent == "OnsetGlu")
            {
                if(child == "Spikes")
                        onset_spikes_glu = value;

                if(child == "Delay")
                        onset_delay_glu = value;

                if(child == "Interval")
                        onset_interval_glu = value;
            }
            if (parent == "OffsetGlu")
            {
                if(child == "Spikes")
                        offset_spikes_glu = value;

                if(child == "Delay")
                        offset_delay_glu = value;

                if(child == "Interval")
                        offset_interval_glu = value;
            }
            if (parent == "OnsetGABA")
            {
                if(child == "Spikes")
                        onset_spikes_GABA = value;

                if(child == "Delay")
                        onset_delay_GABA = value;

                if(child == "Interval")
                        onset_interval_GABA = value;
            }
            if (parent == "AMPA")
            {
                if(child == "gMax")
                        AMPA_gMax = value;
                if(child == "tau1")
                        AMPA_tau1 = value;
                if(child == "tau2")
                        AMPA_tau2 = value;
                if(child == "E")
                        AMPA_E = value;
                if(child == "Delay")
                        AMPA_del = value;
            }
            if (parent == "NMDA")
            {
                if(child == "gMax")
                        NMDA_gMax = value;
                if(child == "tau1")
                        NMDA_tau1 = value;
                if(child == "tau2")
                        NMDA_tau2 = value;
                if(child == "E")
                        NMDA_E = value;
                if(child == "Delay")
                        NMDA_del = value;
            }
            if (parent == "GABA_A")
            {
                if(child == "gMax")
                        GABA_gMax = value;
                if(child == "tau1")
                        GABA_tau1 = value;
                if(child == "tau2")
                        GABA_tau2 = value;
                if(child == "E")
                        GABA_E = value;
                if(child == "Delay")
                        GABA_del = value;
            }
        }
    }

    children = m_refStimulusTree->children();
    for (type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        Gtk::TreeModel::Row row = *iter;

        double duration = row[m_StimulusColumns.m_col_dur];
        int count = row[m_StimulusColumns.m_col_count];
        double gap = row[m_StimulusColumns.m_col_gap];
        
        Synapse AMPA;
        AMPA.gMax = AMPA_gMax; // nS
        AMPA.tau1 = AMPA_tau1; // ms
        AMPA.tau2 = AMPA_tau2; // ms
        AMPA.del = AMPA_del; // ms
        AMPA.E = AMPA_E;

        Synapse NMDA;
        NMDA.gMax = NMDA_gMax; // nS
        NMDA.tau1 = NMDA_tau1; // ms
        NMDA.tau2 = NMDA_tau2; // ms
        NMDA.del = NMDA_del; // ms
        NMDA.E = NMDA_E;

        Synapse GABA_A;
        GABA_A.gMax = GABA_gMax; // nS
        GABA_A.tau1 = GABA_tau1; // ms
        GABA_A.tau2 = GABA_tau2; // ms
        GABA_A.del = GABA_del; // ms
        GABA_A.E = GABA_E;


        onset_spikes_glu = onset_spikes_glu == -1 ? duration/onset_interval_glu : onset_spikes_glu;
        double start = 0;
        for (int c = 0; c < count; ++c)
        {
            for (double onsets = 0; onsets < onset_spikes_glu; onsets+=onset_interval_glu)
            {
                AMPA.spikes.push_back(start+onset_delay_glu+onsets);
                NMDA.spikes.push_back(start+onset_delay_glu+onsets);
            }


            for (double i = 0; i < onset_spikes_GABA; i+=onset_interval_GABA)
            {
                GABA_A.spikes.push_back(start+onset_delay_GABA+i);
            }

            for (int offsets = 0; offsets < offset_spikes_glu; offsets+=offset_interval_glu)
            {
                AMPA.spikes.push_back(start-gap+offset_delay_glu+offsets);
                NMDA.spikes.push_back(start-gap+offset_delay_glu+offsets);
            }

            start += duration+gap;
        }

        Simulation sim = getSimulation();
        sim.synapses.push_back(AMPA);
        sim.synapses.push_back(NMDA);
        sim.synapses.push_back(GABA_A);
        sim.runSim();
        std::vector<double> V = sim.voltagetrace();
        std::vector<double> t = sim.timesteps();
        double cV[V.size()],ct[t.size()];
        for (unsigned int i = 0; i < V.size(); ++i) 
        {
            cV[i] = V[i];
            ct[i] = t[i];
        }

        // Cheat for graph size
        cV[0] = -80;
        cV[1] = -20;

        // Add plot window
        m_pPlot.push_back(new PlotMM::Plot );
        PlotMM::Plot* plot = m_pPlot.back();
        plot->scale(PlotMM::AXIS_RIGHT)->set_enabled(false);
        plot->scale(PlotMM::AXIS_TOP)->set_enabled(false);
        m_VBoxRight.pack_start(*plot, Gtk::PACK_EXPAND_WIDGET, 0);
        Glib::RefPtr<PlotMM::Curve> voltageCurve(new PlotMM::Curve("Voltage"));
        voltageCurve->set_data(ct,cV,V.size());
        plot->add_curve(voltageCurve);
        plot->scale(PlotMM::AXIS_BOTTOM)->set_range(ct[0],ct[t.size()-1],false);
        plot->scale(PlotMM::AXIS_LEFT)->set_range(-80,-20,false);
        plot->replot();
    }
}

