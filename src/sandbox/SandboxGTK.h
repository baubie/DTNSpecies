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
        PlotMM::Plot* m_pPlot;
        Gtk::VBox m_VBoxLeft;
        Gtk::ScrolledWindow m_NetworkTreeWindow;
        Gtk::TreeView m_NetworkList;
        Glib::RefPtr<Gtk::TreeStore> m_refNetworkTree;


        // Tree Model Columns
        class NetworkColumns : public Gtk::TreeModel::ColumnRecord
        {
            public:
                NetworkColumns()
                { add(m_col_name); add(m_col_value); }
                Gtk::TreeModelColumn<Glib::ustring> m_col_name;
                Gtk::TreeModelColumn<double> m_col_value;
        };

        Gtk::CellRendererText m_renderer_value;;
        Gtk::TreeViewColumn m_treeviewcolumn_value;
        void networktree_value_cell_data(Gtk::CellRenderer* renderer, const Gtk::TreeModel::iterator& iter);
        void networktree_value_on_editing_started(Gtk::CellEditable* cell_editable, const Glib::ustring& path);
        void networktree_value_on_edited(const Glib::ustring& path_string, const Glib::ustring& new_text);

        NetworkColumns m_NetworkColumns;

    private:
        void runSim();

};



#endif
