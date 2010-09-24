#include <gtkmm.h>
#include <iostream>

#include "synapse.h"
#include "simulation.h"

Gtk::Window* pMainWindow = 0;

static void on_btnQuit_clicked()
{
    if (pMainWindow)
        pMainWindow->hide();
}

int main(int argc, char **argv) 
{
    Gtk::Main kit(argc, argv);

    // Load the GtkBuilder file and instantiate its widgets
    Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
    try
    {
        refBuilder->add_from_file("sandbox.glade");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }

    //Get the main window
    refBuilder->get_widget("MainWindow", pMainWindow);
    if (pMainWindow)
    {
        Gtk::Button* pBtnQuit = 0;
        refBuilder->get_widget("btnQuit", pBtnQuit);
        if (pBtnQuit)
        {
            pBtnQuit->signal_clicked().connect( sigc::ptr_fun(on_btnQuit_clicked) );
        }

        kit.run(*pMainWindow);
    }

    return 0;
}
