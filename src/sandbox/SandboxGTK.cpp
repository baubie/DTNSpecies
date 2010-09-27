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
        // Add plot window
        m_pPlot = new PlotMM::Plot();
        m_pPlot->scale(PlotMM::AXIS_RIGHT)->set_enabled(false);
        m_pPlot->scale(PlotMM::AXIS_TOP)->set_enabled(false);
        m_pHBoxMain->pack_end(*m_pPlot, Gtk::PACK_EXPAND_WIDGET, 0);

        // Create network parameters TreeView
        m_NetworkTreeWindow.add(m_NetworkList);
        m_NetworkTreeWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
        m_VBoxLeft.pack_start(m_NetworkTreeWindow);
        m_refNetworkTree = Gtk::TreeStore::create(m_NetworkColumns);
        m_NetworkList.set_model(m_refNetworkTree);
        m_NetworkList.append_column("Name", m_NetworkColumns.m_col_name);

        // Special control over the value column
        m_treeviewcolumn_value.set_title("Value");
        m_treeviewcolumn_value.pack_start(m_renderer_value);
        m_NetworkList.append_column(m_treeviewcolumn_value);

        m_treeviewcolumn_value.set_cell_data_func(m_renderer_value,
            sigc::mem_fun(*this, &SandboxGTK::networktree_value_cell_data) );

        m_renderer_value.property_editable() = true;

        m_renderer_value.signal_editing_started().connect(
            sigc::mem_fun(*this, &SandboxGTK::networktree_value_on_editing_started) );

        m_renderer_value.signal_edited().connect(
            sigc::mem_fun(*this, &SandboxGTK::networktree_value_on_edited) );


        m_pHBoxMain->pack_start(m_VBoxLeft, Gtk::PACK_SHRINK, 0);

        // Add default options to tree view
        Simulation sim;
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

        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = "DTN";
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "C";
        childrow[m_NetworkColumns.m_col_value] = sim.C;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "VT";
        childrow[m_NetworkColumns.m_col_value] = sim.VT;

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
            float value = row[m_NetworkColumns.m_col_value];
            char buffer[8];
            sprintf(buffer, "%.2f", value);
            Glib::ustring view_text = buffer;
            m_renderer_value.property_text() = view_text;
        } else {
            m_renderer_value.property_text() = "";
        }
    }
}
void SandboxGTK::networktree_value_on_editing_started(Gtk::CellEditable* cell_editable, const Glib::ustring& path)
{
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
    runSim();
}


void SandboxGTK::on_btnQuit_clicked()
{
    delete m_pPlot;
    delete m_pBtnQuit;
    delete m_pBtnRefresh;
    delete m_pHBoxMain;
    hide();
}

void SandboxGTK::on_btnRefresh_clicked()
{
    runSim();
}

void SandboxGTK::runSim()
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
            std::cout << parent << "->" << child << "->" << value << std::endl;
            if (parent == "Simulation")
            {
                if(child == "T")
                        sim.T = value;

                if(child == "dt")
                        sim.dt = value;
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

    Synapse AMPA;
    AMPA.gMax = 25; // nS
    AMPA.tau1 = 0.8; // ms
    AMPA.tau2 = 3; // ms
    AMPA.del = 25; // ms
    AMPA.E = 0;
    AMPA.spikes.push_back(0);

    Synapse::reset();
    Synapse::dt = sim.dt;
    Synapse::prepare(AMPA.tau1,AMPA.tau2);

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
    m_pPlot->scale(PlotMM::AXIS_BOTTOM)->set_range(ct[0],ct[t.size()-1],false);
    m_pPlot->scale(PlotMM::AXIS_LEFT)->set_range(-75,50,false);
    m_pPlot->add_curve(voltageCurve);
    m_pPlot->replot();
}

