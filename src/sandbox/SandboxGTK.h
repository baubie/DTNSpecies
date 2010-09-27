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

class SandboxGTK : public Gtk::Window
{
    public:
        SandboxGTK(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
        virtual ~SandboxGTK();

    protected:
        // Signal handlers
        void on_btnQuit_clicked();
        void on_btnRefresh_clicked();

        // Child Widgets (from glade file)
        Glib::RefPtr<Gtk::Builder> m_refGlade;
        Gtk::ToolButton* m_pBtnQuit;
        Gtk::ToolButton* m_pBtnRefresh;
        Gtk::HBox* m_pHBoxMain;

        // Child Widgets (created in c++)
        std::vector<PlotMM::Plot*> m_pPlot;
        Gtk::VBox m_VBoxLeft;
        Gtk::VBox m_VBoxRight;
        Gtk::ScrolledWindow m_LeftScrollWindow;
        Gtk::TreeView m_NetworkList;
        Gtk::TreeView m_StimulusList;
        Glib::RefPtr<Gtk::TreeStore> m_refNetworkTree;
        Glib::RefPtr<Gtk::ListStore> m_refStimulusTree;


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
                { add(m_col_name); add(m_col_dur); add(m_col_count); add(m_col_gap); }
                Gtk::TreeModelColumn<Glib::ustring> m_col_name;
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
        void deletePlots();
};



#endif
