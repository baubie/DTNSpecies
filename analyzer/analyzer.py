import sys
import pygtk
import gtk
import gtk.glade
from LogFile import LogFile

import matplotlib
matplotlib.use('GTK')   
from matplotlib.figure import Figure   
from matplotlib.axes import Subplot   
from matplotlib.backends.backend_gtk import FigureCanvasGTK, NavigationToolbar   

class AnalyzerGTK:

    def __init__(self):
        
        # Set the Glade file
        self.gladefile = "analyzer.glade"
        self.wTree = gtk.glade.XML(self.gladefile)

        # Get the Main Window and connect the "destroy" event
        self.window = self.wTree.get_widget("MainWindow")
        if (self.window):
            self.window.connect("destroy", gtk.main_quit)

        # Hook up the signals
        dic = { 
            "on_menuOpen_activate": self.menuOpen_activate,
            "on_menuQuit_activate": self.menuQuit_activate,
            "on_menuAbout_activate": self.menuAbout_activate,
            "on_MainWindow_destroy": gtk.main_quit,
            "on_NetworkList_row_activated": self.plotRows,

            "on_tbClear_clicked": self.clearPlot
        }

        self.wTree.signal_autoconnect(dic)

        # Create a LogFile object to handle data
        self.logfile = LogFile()

        # Create an empty plot window
        self.figure = Figure(figsize=(6,4), dpi=72)   
        self.axis = self.figure.add_subplot(111)   
        self.axis.set_xlabel('Duration')   
        self.axis.set_ylabel('Mean Number of Spikes')   
        self.axis.grid(True)   
        self.canvas = FigureCanvasGTK(self.figure) # a gtk.DrawingArea   
        self.canvas.show()   
        self.graphview = self.wTree.get_widget("vpanel")   
        self.graphview.add2(self.canvas)

    def plotRows(self, widget, path, column):
        (model, pathlist) = self.treeview.get_selection().get_selected_rows()
        for p in pathlist:
            treeiter = self.liststore.get_iter(p)
            d = self.logfile.getresults(self.liststore.get_value(treeiter,0)) # +1 because we skip the template in the liststore
            self.axis.plot(d.keys(), d.values())
        self.canvas.draw()

    def clearPlot(self, widget):
        self.axis.cla()
        self.axis.set_xlabel('Duration')   
        self.axis.set_ylabel('Mean Number of Spikes')   
        self.axis.grid(True)   
        self.canvas.draw()

    def menuOpen_activate(self, widget):
        chooser = gtk.FileChooserDialog(title="Open Log File", 
                                    action=gtk.FILE_CHOOSER_ACTION_OPEN,
                                    buttons=(gtk.STOCK_CANCEL,gtk.RESPONSE_CANCEL,
                                             gtk.STOCK_OPEN, gtk.RESPONSE_OK))
        response = chooser.run()
        if response == gtk.RESPONSE_OK:
            filename = chooser.get_filename()
            if self.logfile.open(filename):
                # Data file opened
                # Populate the ListStore for our table
                self.liststore = gtk.ListStore(int, *([float] *len(self.logfile.params())))
                self.treeview = self.wTree.get_widget("NetworkList")
                self.treeview.set_model(self.liststore)
                self.treeview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)

                # Add columns
                textrenderer = gtk.CellRendererText()
                col = 1
                for p in self.logfile.params():
                    column = gtk.TreeViewColumn(p, textrenderer, text=col)
                    column.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
                    column.set_resizable(True)
                    column.set_clickable(True)
                    column.set_sort_column_id(col)
                    self.treeview.append_column(column)
                    col = col + 1

                # Add data
                for n in self.logfile.networkdefs():
                    itt = self.liststore.append(n)

        chooser.destroy()

    def menuAbout_activate(self, widget):
        aboutDialog = self.wTree.get_widget("AboutDialog")
        response = aboutDialog.run()
        if response == gtk.RESPONSE_CANCEL:
            aboutDialog.hide()

    def menuQuit_activate(self, widget):
        gtk.main_quit()

if __name__ == "__main__":
    hwg = AnalyzerGTK()
    gtk.main()
