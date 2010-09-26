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

        // Widgets
        Glib::RefPtr<Gtk::Builder> m_refGlade;
        PlotMM::Plot* m_pPlot;
        Gtk::ToolButton* m_pBtnQuit;
        Gtk::ToolButton* m_pBtnRefresh;
        Gtk::HBox* m_pHboxMain;
};
