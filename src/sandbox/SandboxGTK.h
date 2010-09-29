#ifndef SANDBOXGTK_H
#define SANDBOXGTK_H


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

struct NetworkFile
{
    Simulation sim;
    Synapse AMPA;
    Synapse NMDA;
    Synapse GABA_A;
};

struct Stimulus
{
    double dur;
    int count;
    double gap;
};

typedef std::vector<Stimulus> Stimuli;

class SandboxGTK : public Gtk::Window
{
    public:
        SandboxGTK(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
        virtual ~SandboxGTK();

    protected:
        // Signal handlers
        void on_btnQuit_clicked();
        void on_btnRunStimuli_clicked();
        void on_btnSaveNetwork_clicked();
        void on_btnOpenNetwork_clicked();
        void on_btnSaveStimuli_clicked();
        void on_btnOpenStimuli_clicked();
        void on_btnAddStimulus_clicked();
        void on_btnDeleteStimulus_clicked();

        // Child Widgets (from glade file)
        Glib::RefPtr<Gtk::Builder> m_refGlade;
        Gtk::ToolButton* m_pBtnQuit;
        Gtk::ToolButton* m_pBtnRefresh;
        Gtk::ToolButton* m_pBtnSaveNetwork;
        Gtk::ToolButton* m_pBtnOpenNetwork;
        Gtk::ToolButton* m_pBtnSaveStimuli;
        Gtk::ToolButton* m_pBtnOpenStimuli;

        Gtk::ToggleToolButton* m_pBtnSummary;
        Gtk::ToggleToolButton* m_pBtnJitter;

        Gtk::HBox* m_pHBoxMain;

        // Child Widgets (created in c++)
        std::vector<PlotMM::Plot*> m_pPlot;
        Gtk::VBox m_VBoxLeft;
        Gtk::VBox m_VBoxRight;
        Gtk::HBox m_HBoxStimEdit;
        Gtk::ScrolledWindow m_LeftScrollWindow;
        Gtk::TreeView m_NetworkList;
        Gtk::TreeView m_StimulusList;
        Gtk::Button m_btnAddStimulus;
        Gtk::Button m_btnDeleteStimulus;
        Glib::RefPtr<Gtk::TreeStore> m_refNetworkTree;
        Glib::RefPtr<Gtk::ListStore> m_refStimulusTree;
        Glib::RefPtr<Gtk::TreeSelection> m_refStimulusSelection;


        // Network Tree Model Columns
        class NetworkColumns : public Gtk::TreeModel::ColumnRecord
        {
            public:
                NetworkColumns()
                { add(m_col_name); add(m_col_value); }
                Gtk::TreeModelColumn<Glib::ustring> m_col_name;
                Gtk::TreeModelColumn<double> m_col_value;
        };

        class StimulusColumns : public Gtk::TreeModel::ColumnRecord
        {
            public:
                StimulusColumns()
                { add(m_col_dur); add(m_col_count); add(m_col_gap); }
                Gtk::TreeModelColumn<int> m_col_dur;
                Gtk::TreeModelColumn<int> m_col_count;
                Gtk::TreeModelColumn<float> m_col_gap;
        };

        Gtk::CellRendererText m_renderer_value;;
        Gtk::TreeViewColumn m_treeviewcolumn_value;
        void networktree_value_cell_data(Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);
        void networktree_value_on_edited(const Glib::ustring& path_string, const Glib::ustring& new_text);

        NetworkColumns m_NetworkColumns;
        StimulusColumns m_StimulusColumns;

    private:
        void runSim();
        Simulation getSimulation();
        Synapse getSynapse(const std::string& name);
        void deletePlots();
        void populateNetworkTree(const NetworkFile &nf);
        void populateStimulusTree(const Stimuli &sf);
        void updateStimDeleteButton();
};



#endif
