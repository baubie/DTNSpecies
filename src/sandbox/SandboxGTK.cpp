#include "SandboxGTK.h"

SandboxGTK::SandboxGTK(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Window(cobject),
  m_refGlade(refGlade),
  m_pBtnQuit(0),
  m_pBtnRefresh(0),
  m_btnAddStimulus(Gtk::Stock::ADD),
  m_btnDeleteStimulus(Gtk::Stock::DELETE)
{

    set_title("Duration-Tuned Neuron Sandbox (Version 0.3)");

    // Setup the toolbar
    m_refGlade->get_widget("btnQuit", m_pBtnQuit);
    if (m_pBtnQuit)
        m_pBtnQuit->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnQuit_clicked));
        
    m_refGlade->get_widget("btnRunStimuli", m_pBtnRefresh);
    if (m_pBtnRefresh)
        m_pBtnRefresh->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnRunStimuli_clicked));

    m_refGlade->get_widget("btnSaveNetwork", m_pBtnSaveNetwork);
    if (m_pBtnSaveNetwork)
        m_pBtnSaveNetwork->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnSaveNetwork_clicked));

    m_refGlade->get_widget("btnOpenNetwork", m_pBtnOpenNetwork);
    if (m_pBtnOpenNetwork)
        m_pBtnOpenNetwork->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnOpenNetwork_clicked));

    m_refGlade->get_widget("btnSaveStimuli", m_pBtnSaveStimuli);
    if (m_pBtnSaveStimuli)
        m_pBtnSaveStimuli->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnSaveStimuli_clicked));

    m_refGlade->get_widget("btnOpenStimuli", m_pBtnOpenStimuli);
    if (m_pBtnOpenStimuli)
        m_pBtnOpenStimuli->signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnOpenStimuli_clicked));

    m_refGlade->get_widget("btnSummary", m_pBtnSummary);
    m_refGlade->get_widget("btnJitter", m_pBtnJitter);

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


        // Create stimulus TreeView
        m_refStimulusTree = Gtk::ListStore::create(m_StimulusColumns);
        m_StimulusList.set_model(m_refStimulusTree);
        m_StimulusList.append_column_editable("Show", m_StimulusColumns.m_col_voltage);
        m_StimulusList.append_column_editable("Dur", m_StimulusColumns.m_col_dur);
        m_StimulusList.append_column_editable("Count", m_StimulusColumns.m_col_count);
        m_StimulusList.append_column_editable("Gap", m_StimulusColumns.m_col_gap);

        // Default network
        NetworkFile nf;
        nf.sim.defaultparams();
        nf.AMPA.onsetCount = 1;
        nf.AMPA.offsetCount = 1;
        nf.AMPA.onsetDel = 15;
        nf.AMPA.offsetDel = 15;
        nf.AMPA.onsetInterval = 1;
        nf.AMPA.offsetInterval = 1;
        nf.AMPA.tau1 = 0.01;
        nf.AMPA.tau2 = 2;
        nf.AMPA.gMax = 6;
        nf.AMPA.E = 0;
        nf.NMDA.onsetCount = 1;
        nf.NMDA.offsetCount = 1;
        nf.NMDA.onsetDel = 20;
        nf.NMDA.offsetDel = 20;
        nf.NMDA.onsetInterval = 1;
        nf.NMDA.offsetInterval = 1;
        nf.NMDA.tau1 = 2;
        nf.NMDA.tau2 = 5;
        nf.NMDA.gMax = 3;
        nf.NMDA.E = -10;
        nf.GABA_A.onsetCount = -1;
        nf.GABA_A.offsetCount = 0;
        nf.GABA_A.onsetDel = 0;
        nf.GABA_A.onsetInterval = 1;
        nf.GABA_A.offsetInterval = 1;
        nf.GABA_A.tau1 = 0.01;
        nf.GABA_A.tau2 = 2;
        nf.GABA_A.gMax = 3;
        nf.GABA_A.E = -80;

        // Default stimuli
        Stimuli sf;
        Stimulus s;
        for (int i = 1; i < 10; i++)
        {
            s.voltage = true;
            s.dur = i;
            s.count = 1;
            sf.push_back(s);
        }

        populateNetworkTree(nf);
        populateStimulusTree(sf);

        // Add Stimulus Buttons
        m_btnAddStimulus.signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnAddStimulus_clicked));
        m_btnDeleteStimulus.signal_clicked().connect(sigc::mem_fun(*this, &SandboxGTK::on_btnDeleteStimulus_clicked));
        m_btnDeleteStimulus.set_sensitive(false);
        m_HBoxStimEdit.pack_start(m_btnAddStimulus);
        m_HBoxStimEdit.pack_start(m_btnDeleteStimulus);
        m_refStimulusSelection = m_StimulusList.get_selection();
        m_refStimulusSelection->signal_changed().connect(
            sigc::mem_fun(*this, &SandboxGTK::updateStimDeleteButton)
        );

        // Add Fill left VBox 
        m_VBoxLeft.pack_start(m_NetworkList,false,false);
        m_LeftScrollWindow.add(m_VBoxLeft);
        m_LeftScrollWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
        m_VBoxRight.pack_end(m_HBoxStimEdit,false,false);
        m_VBoxRight.pack_end(m_StimulusList,true,true);
        m_RightScrollWindow.add(m_VBoxRight);
        m_RightScrollWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

        // Add VBoxes to main window
        m_pHBoxMain->pack_start(m_LeftScrollWindow, Gtk::PACK_SHRINK, 0);
        m_pHBoxMain->pack_start(m_VBoxMiddle, Gtk::PACK_EXPAND_WIDGET, 0);
        m_pHBoxMain->pack_start(m_RightScrollWindow, Gtk::PACK_SHRINK, 0);
    }

    show_all_children();

}

void SandboxGTK::updateStimDeleteButton()
{
    Gtk::TreeModel::iterator iter = m_refStimulusSelection->get_selected();
    if(iter) m_btnDeleteStimulus.set_sensitive(true);
    else m_btnDeleteStimulus.set_sensitive(false);
}

void SandboxGTK::populateStimulusTree(const Stimuli &sf)
{
    // Add stimuli
    m_refStimulusTree->clear();
    Gtk::TreeModel::Row row;
    for (Stimuli::const_iterator i = sf.begin(); i != sf.end(); i++)
    {
        row = *(m_refStimulusTree->append());
        row[m_StimulusColumns.m_col_voltage] = i->voltage;
        row[m_StimulusColumns.m_col_dur] = i->dur;
        row[m_StimulusColumns.m_col_count] = i->count;
        row[m_StimulusColumns.m_col_gap] = i->gap;
    }
    m_refStimulusTree->set_sort_column(m_StimulusColumns.m_col_dur,Gtk::SORT_ASCENDING);
}

void SandboxGTK::populateNetworkTree(const NetworkFile &nf)
{

    // Add default options to tree view
    Gtk::TreeModel::Row row;
    Gtk::TreeModel::Row childrow;
    m_refNetworkTree->clear();
    row = *(m_refNetworkTree->append());
    row[m_NetworkColumns.m_col_name] = "Simulation";
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "T";
    childrow[m_NetworkColumns.m_col_value] = nf.sim.T;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "dt";
    childrow[m_NetworkColumns.m_col_value] = nf.sim.dt;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "delay";
    childrow[m_NetworkColumns.m_col_value] = nf.sim.del;

    row = *(m_refNetworkTree->append());
    row[m_NetworkColumns.m_col_name] = "DTN";
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "C";
    childrow[m_NetworkColumns.m_col_value] =nf.sim.C;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "gL";
    childrow[m_NetworkColumns.m_col_value] =nf.sim.gL;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "EL";
    childrow[m_NetworkColumns.m_col_value] =nf.sim.EL;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "VT";
    childrow[m_NetworkColumns.m_col_value] =nf.sim.VT;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "dT";
    childrow[m_NetworkColumns.m_col_value] =nf.sim.dT;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "a";
    childrow[m_NetworkColumns.m_col_value] =nf.sim.a;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "tauw";
    childrow[m_NetworkColumns.m_col_value] =nf.sim.tau_w;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "b";
    childrow[m_NetworkColumns.m_col_value] =nf.sim.b;
    childrow = *(m_refNetworkTree->append(row.children()));
    childrow[m_NetworkColumns.m_col_name] = "Vr";
    childrow[m_NetworkColumns.m_col_value] =nf.sim.Vr;

    std::vector<std::string> syns;
    syns.push_back("AMPA"); syns.push_back("NMDA"); syns.push_back("GABA_A");

    for(std::vector<std::string>::iterator i = syns.begin(); i != syns.end(); i++)
    {
        Synapse syn;
        if (*i == "AMPA") syn = nf.AMPA;
        if (*i == "NMDA") syn = nf.NMDA;
        if (*i == "GABA_A") syn = nf.GABA_A;
        row = *(m_refNetworkTree->append());
        row[m_NetworkColumns.m_col_name] = *i;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "gMax";
        childrow[m_NetworkColumns.m_col_value] = syn.gMax;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "tau1";
        childrow[m_NetworkColumns.m_col_value] = syn.tau1;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "tau2";
        childrow[m_NetworkColumns.m_col_value] = syn.tau2;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "E";
        childrow[m_NetworkColumns.m_col_value] = syn.E;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Onset Delay";
        childrow[m_NetworkColumns.m_col_value] = syn.onsetDel;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Onset Count";
        childrow[m_NetworkColumns.m_col_value] = syn.onsetCount;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Onset Interval";
        childrow[m_NetworkColumns.m_col_value] = syn.onsetInterval;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Offset Delay";
        childrow[m_NetworkColumns.m_col_value] = syn.offsetDel;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Offset Count";
        childrow[m_NetworkColumns.m_col_value] = syn.offsetCount;
        childrow = *(m_refNetworkTree->append(row.children()));
        childrow[m_NetworkColumns.m_col_name] = "Offset Interval";
        childrow[m_NetworkColumns.m_col_value] = syn.offsetInterval;
    }

}

void SandboxGTK::on_btnAddStimulus_clicked()
{
    Gtk::TreeModel::Row row;
    row = *(m_refStimulusTree->append());
    row[m_StimulusColumns.m_col_dur] = 0;
    row[m_StimulusColumns.m_col_count] = 0;
    row[m_StimulusColumns.m_col_gap] = 0;
    m_refStimulusTree->set_sort_column(m_StimulusColumns.m_col_dur,Gtk::SORT_ASCENDING);
}

void SandboxGTK::on_btnDeleteStimulus_clicked()
{
    Gtk::TreeModel::iterator iter = m_refStimulusSelection->get_selected();
    if(iter)
    {
       m_refStimulusTree->erase(iter);
    }
}

void SandboxGTK::on_btnSaveNetwork_clicked()
{

    Gtk::FileChooserDialog dialog("Select a Network File",
        Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);

    // Add response buttons to the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

    // Add filters
    Gtk::FileFilter filter_text;
    filter_text.set_name("Network Files");
    filter_text.add_pattern("*.network");

    // Show the dialog
    int result = dialog.run();

    switch(result)
    {
        case(Gtk::RESPONSE_OK):
            NetworkFile nf;
            nf.sim = getSimulation();
            nf.AMPA = getSynapse("AMPA");
            nf.NMDA = getSynapse("NMDA");
            nf.GABA_A = getSynapse("GABA_A");
            std::string filename = dialog.get_filename();
            std::ofstream out(filename.c_str(), std::ios::binary);
            if (out.good())
            {
                out.write(reinterpret_cast<char*>(&nf), sizeof(nf));
            }
            break;
    }
}

void SandboxGTK::on_btnOpenNetwork_clicked()
{
    Gtk::FileChooserDialog dialog("Select a Network File",
        Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    // Add response buttons to the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

    // Add filters
    Gtk::FileFilter filter_text;
    filter_text.set_name("Network Files");
    filter_text.add_pattern("*.network");

    // Show the dialog
    int result = dialog.run();

    switch(result)
    {
        case(Gtk::RESPONSE_OK):
            NetworkFile nf;
            std::string filename = dialog.get_filename();
            std::ifstream in(filename.c_str(), std::ios::binary);
            if (in.good())
            {
                in.read(reinterpret_cast<char*>(&nf), sizeof(nf));
                populateNetworkTree(nf);
            }
            break;
    }
}

void SandboxGTK::on_btnSaveStimuli_clicked()
{

    Gtk::FileChooserDialog dialog("Select a Stimuli File",
        Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);

    // Add response buttons to the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

    // Add filters
    Gtk::FileFilter filter_text;
    filter_text.set_name("Stimulus Files");
    filter_text.add_pattern("*.stimuli");

    // Show the dialog
    int result = dialog.run();

    switch(result)
    {
        case(Gtk::RESPONSE_OK):
            std::string filename = dialog.get_filename();
            std::ofstream out(filename.c_str(), std::ios::binary);
            if (out.good())
            {
                typedef Gtk::TreeModel::Children type_children;
                type_children children = m_refStimulusTree->children();
                unsigned int num = children.size();
                out.write(reinterpret_cast<char*>(&num), sizeof(num));
                for (type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
                {
                    Gtk::TreeModel::Row row = *iter;
                    Stimulus stim;
                    stim.dur = row[m_StimulusColumns.m_col_dur];
                    stim.count = row[m_StimulusColumns.m_col_count];
                    stim.gap = row[m_StimulusColumns.m_col_gap];
                    out.write(reinterpret_cast<char*>(&stim), sizeof(stim));
                }
            }
            break;
    }
}

void SandboxGTK::on_btnOpenStimuli_clicked()
{
    Gtk::FileChooserDialog dialog("Select a Stimuli File",
        Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    // Add response buttons to the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

    // Add filters
    Gtk::FileFilter filter_text;
    filter_text.set_name("Stimuli Files");
    filter_text.add_pattern("*.stimuli");

    // Show the dialog
    int result = dialog.run();

    switch(result)
    {
        case(Gtk::RESPONSE_OK):
            Stimuli sf;
            unsigned int num;
            std::string filename = dialog.get_filename();
            std::ifstream in(filename.c_str(), std::ios::binary);
            if (in.good())
            {
                in.read(reinterpret_cast<char*>(&num), sizeof(num));
                for (unsigned int i = 0; i < num; ++i)
                {
                    Stimulus s;
                    in.read(reinterpret_cast<char*>(&s), sizeof(s));
                    sf.push_back(s);
                }
                populateStimulusTree(sf);
            }
            break;
    }
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

Synapse SandboxGTK::getSynapse(const std::string& name)
{
    Synapse r;
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
            if (name.c_str() == parent)
            {
                if(child == "gMax")
                        r.gMax = value;
                if(child == "tau1")
                        r.tau1 = value;
                if(child == "tau2")
                        r.tau2 = value;
                if(child == "E")
                        r.E = value;
                if(child == "Onset Delay")
                        r.onsetDel = value;
                if(child == "Offset Delay")
                        r.offsetDel = value;
                if(child == "Onset Count")
                        r.onsetCount = value;
                if(child == "Offset Count")
                        r.offsetCount = value;
                if(child == "Onset Interval")
                        r.onsetInterval = value;
                if(child == "Offset Interval")
                        r.offsetInterval = value;
            }
        }
    }
    if (r.onsetCount == 1) r.onsetInterval = 1;
    if (r.offsetCount == 1) r.offsetInterval = 1;
    return r;
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
                        sim.Vr = value;
            }
        }

    }
    return sim;
}
void SandboxGTK::runSim()
{

    deletePlots();

    
    std::vector<double> jitter;
    if (m_pBtnJitter->get_active())
    {
        jitter.push_back(-0.2);
        jitter.push_back(0.2);
        jitter.push_back(-0.1);
        jitter.push_back(0.1);
        jitter.push_back(0);
    }
    else
    {
        jitter.push_back(0);
    }

    typedef Gtk::TreeModel::Children type_children;
    type_children children = m_refStimulusTree->children();
    double cSummaryX[children.size()];
    double cSummaryY[children.size()];
    int stimNumber = 0;
    double maxSummary = 0;
    for (type_children::iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        Gtk::TreeModel::Row row = *iter;

        bool showVoltage = row[m_StimulusColumns.m_col_voltage];
        double duration = row[m_StimulusColumns.m_col_dur];
        int count = row[m_StimulusColumns.m_col_count];
        double gap = row[m_StimulusColumns.m_col_gap];

        
        std::vector< std::vector<double> > Vs;
        std::vector< std::vector<double> >::iterator it_Vs;
        std::vector<double> t;
        std::vector<double> V;


        double meanSpikes = 0;
        for (unsigned int j = 0; j < jitter.size(); j++)
        {
            Synapse AMPA = getSynapse("AMPA");
            Synapse NMDA = getSynapse("NMDA");
            Synapse GABA_A = getSynapse("GABA_A");

            AMPA.onsetCount = AMPA.onsetCount == -1 ? duration/AMPA.onsetInterval : AMPA.onsetCount;
            NMDA.onsetCount = NMDA.onsetCount == -1 ? duration/NMDA.onsetInterval : NMDA.onsetCount;
            GABA_A.onsetCount = GABA_A.onsetCount == -1 ? duration/GABA_A.onsetInterval : GABA_A.onsetCount;
            double start = 0;
            for (int c = 0; c < count; ++c)
            {
                for (double onsets = 0; onsets < AMPA.onsetCount; onsets+=AMPA.onsetInterval)
                {
                    AMPA.spikes.push_back(start+AMPA.onsetDel+onsets+jitter[j]);
                }
                for (double onsets = 0; onsets < NMDA.onsetCount; onsets+=NMDA.onsetInterval)
                {
                    NMDA.spikes.push_back(start+NMDA.onsetDel+onsets+jitter[j]);
                }
                for (double onsets = 0; onsets < GABA_A.onsetCount; onsets+=GABA_A.onsetInterval)
                {
                    GABA_A.spikes.push_back(start+GABA_A.onsetDel+onsets+jitter[j]);
                }
                for (double offsets = 0; offsets < AMPA.offsetCount; offsets+=AMPA.offsetInterval)
                {
                    AMPA.spikes.push_back(start+duration+AMPA.offsetDel+offsets);
                }
                for (double offsets = 0; offsets < NMDA.offsetCount; offsets+=NMDA.offsetInterval)
                {
                    NMDA.spikes.push_back(start+duration+NMDA.offsetDel+offsets);
                }
                start += duration+gap;
            }
            Simulation sim = getSimulation();
            sim.synapses.push_back(AMPA);
            sim.synapses.push_back(NMDA);
            sim.synapses.push_back(GABA_A);
            sim.runSim();
            V = sim.voltagetrace();
            Vs.push_back(V);
            t = sim.timesteps();
            meanSpikes += (double)sim.spikes().size()/(double)jitter.size();
        }
        double cV[t.size()],ct[t.size()],cStimX[2],cStimY[2];
        cStimX[0] = 0;
        cStimX[1] = duration;
        cStimY[0] = -75;
        cStimY[1] = -75;
        for (unsigned int i = 0; i < t.size(); ++i) 
        {
            cV[i] = 0;
        }
        for (it_Vs = Vs.begin(); it_Vs != Vs.end(); it_Vs++)
        {
            for (unsigned int i = 0; i < t.size(); ++i) 
            {
                cV[i] += ((*it_Vs)[i])/jitter.size();
            }
        }
        for (unsigned int i = 0; i < t.size(); ++i) 
        {
            if (cV[i] > -20) cV[i] = -20;
                ct[i] = t[i];
        }
        cSummaryX[stimNumber] = duration;
        cSummaryY[stimNumber] = meanSpikes;
        maxSummary = meanSpikes > maxSummary ? meanSpikes : maxSummary;
        stimNumber++;

        // Cheat for graph size
        cV[0] = -80;
        cV[1] = -20;

        if (showVoltage) 
        {
            // Add plot window
            m_pPlot.push_back(new PlotMM::Plot );
            PlotMM::Plot* plot = m_pPlot.back();
            plot->scale(PlotMM::AXIS_RIGHT)->set_enabled(false);
            plot->scale(PlotMM::AXIS_TOP)->set_enabled(false);
            m_VBoxMiddle.pack_end(*plot, Gtk::PACK_EXPAND_WIDGET, 0);
            Glib::RefPtr<PlotMM::Curve> voltageCurve(new PlotMM::Curve("Voltage"));
            voltageCurve->set_data(ct,cV,t.size());
            Glib::RefPtr<PlotMM::Curve> stimulusCurve(new PlotMM::Curve("Stimulus"));
            stimulusCurve->set_data(cStimX,cStimY,2);
            plot->add_curve(voltageCurve);
            plot->add_curve(stimulusCurve);
            plot->scale(PlotMM::AXIS_BOTTOM)->set_range(ct[0],ct[t.size()-1],false);
            plot->scale(PlotMM::AXIS_LEFT)->set_range(-80,-20,false);
            plot->replot();
        }
    }
    if (m_pBtnSummary->get_active())
    {
        // Add summary plot window
        m_pPlot.push_back(new PlotMM::Plot );
        PlotMM::Plot* plot = m_pPlot.back();
        plot->scale(PlotMM::AXIS_RIGHT)->set_enabled(false);
        plot->scale(PlotMM::AXIS_TOP)->set_enabled(false);
        m_VBoxRight.pack_start(*plot, Gtk::PACK_EXPAND_WIDGET, 0);
        Glib::RefPtr<PlotMM::Curve> voltageCurve(new PlotMM::Curve("Mean Spikes"));
        voltageCurve->set_data(cSummaryX,cSummaryY,stimNumber);
        plot->scale(PlotMM::AXIS_BOTTOM)->set_range(cSummaryX[0],cSummaryX[stimNumber-1],false);
        plot->scale(PlotMM::AXIS_LEFT)->set_range(0,maxSummary+0.5,false);
        plot->add_curve(voltageCurve);
        plot->replot();
    }
}

