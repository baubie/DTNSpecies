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
        
    m_refGlade->get_widget("btnRefresh", m_pBtnRefresh);
    if (m_pBtnRefresh)
        m_pBtnRefresh->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnRefresh_clicked));

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
        sim.T = 50;
        sim.dt = 0.1;
        sim.defaultparams();
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
        childrow[m_NetworkColumns.m_col_name] = "VT";
        childrow[m_NetworkColumns.m_col_value] = sim.VT;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "OnsetGlu";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Spikes";
        childrow[m_NetworkColumns.m_col_value] = 1;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Delay";
        childrow[m_NetworkColumns.m_col_value] = 0;

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "OffsetGlu";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Spikes";
        childrow[m_NetworkColumns.m_col_value] = 1;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Delay";
        childrow[m_NetworkColumns.m_col_value] = 0;

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
        childrow[m_NetworkColumns.m_col_value] = 3;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "E";
        childrow[m_NetworkColumns.m_col_value] = 0;

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
        childrow[m_NetworkColumns.m_col_value] = 3;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "E";
        childrow[m_NetworkColumns.m_col_value] = -75;


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
        row[m_StimulusColumns.m_col_name] = "25 ms";
        row[m_StimulusColumns.m_col_dur] = 25;
        row[m_StimulusColumns.m_col_count] = 1;
        row[m_StimulusColumns.m_col_gap] = 0;

        row = *(m_refStimulusTree->append());
        row[m_StimulusColumns.m_col_name] = "1 ms Chain";
        row[m_StimulusColumns.m_col_dur] = 1;
        row[m_StimulusColumns.m_col_count] = 5;
        row[m_StimulusColumns.m_col_gap] = 1;


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

void SandboxGTK::on_btnRefresh_clicked()
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

                if(child == "VT")
                        sim.VT = value;
            }
        }

    }
    return sim;
}
void SandboxGTK::runSim()
{

    int onset_spikes = 0;
    int offset_spikes = 0;
    int onset_spikes_GABA = 0;
    double onset_delay = 0;
    double offset_delay = 0;
    double onset_delay_GABA = 0;
    double onset_interval_GABA = 0;

    double AMPA_gMax = 0;
    double AMPA_tau1 = 0;
    double AMPA_tau2 = 0;
    double AMPA_E = 0;

    double GABA_gMax = 0;
    double GABA_tau1 = 0;
    double GABA_tau2 = 0;
    double GABA_E = 0;

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
                        onset_spikes = value;

                if(child == "Delay")
                        onset_delay = value;
            }
            if (parent == "OffsetGlu")
            {
                if(child == "Spikes")
                        offset_spikes = value;

                if(child == "Delay")
                        offset_delay = value;
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
        AMPA.del = 0; // ms
        AMPA.E = AMPA_E;

        Synapse GABA_A;
        GABA_A.gMax = GABA_gMax; // nS
        GABA_A.tau1 = GABA_tau1; // ms
        GABA_A.tau2 = GABA_tau2; // ms
        GABA_A.del = 0; // ms
        GABA_A.E = GABA_E;

        double start = 0;
        for (int c = 0; c < count; ++c)
        {
            for (double onsets = 0; onsets < onset_spikes; onsets++)
                AMPA.spikes.push_back(start+onset_delay+onsets);


            if (onset_spikes_GABA == -1)
            {
                for (double i = 0; i < duration; i+=onset_interval_GABA)
                    GABA_A.spikes.push_back(start+onset_delay_GABA+i);
            } else {
                for (double i = 0; i < onset_spikes_GABA; i+=onset_interval_GABA)
                    GABA_A.spikes.push_back(start+onset_delay_GABA+i);
            }
            start += duration+gap;
        }
        for (int offsets = 0; offsets < offset_spikes; offsets++)
            AMPA.spikes.push_back(start-gap+offset_delay+offsets);

        Simulation sim = getSimulation();
        sim.synapses.push_back(AMPA);
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
        cV[1] = 40;

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
        plot->scale(PlotMM::AXIS_LEFT)->set_range(-80,40,false);
        plot->replot();
    }
}

