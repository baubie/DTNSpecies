#!/usr/bin/python

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

def unique(seq, idfun=None): 
    if idfun is None:
        def idfun(x): return x
    seen = {}
    result = []
    for item in seq:
        marker = idfun(item)
        if marker in seen: continue
        seen[marker] = 1
        result.append(item)
    return result

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
            
            "on_btnClearFilter_clicked": self.clearFilter,
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
        self.graphview = self.wTree.get_widget("TopVPanel")   
        self.graphview.add2(self.canvas)
        self.maxSpikes = 1

    def plotRows(self, widget, path, column):
        (model, pathlist) = self.treeview.get_selection().get_selected_rows()
        for p in pathlist:
            treeiter = self.liststore.get_iter(p)
            X = self.logfile.getdurs()
            Y = self.logfile.getresults(self.liststore.get_value(treeiter,0))
            tmp = Y[:]
            tmp.append(self.maxSpikes)
            self.maxSpikes = max(tmp)
            self.axis.plot(X, Y)
            self.axis.set_ylim( (0, self.maxSpikes+0.1) )
        self.canvas.draw()

    def clearPlot(self, widget):
        self.maxSpikes = 1
        self.axis.cla()
        self.axis.set_xlabel('Duration')   
        self.axis.set_ylabel('Mean Number of Spikes')   
        self.axis.grid(True)   
        self.canvas.draw()

    def clearFilter(self, widget):
        for i in range(len(self.filtercboxes)):
            self.filtercboxes[i].set_active(0)

    def applyFilter(self,model,iter):
        show = True
        for i in range(len(self.filtercboxes)):
            f = self.filtercboxes[i].get_active_text()
            show = show and (f == "All" or f == str(self.liststore.get_value(iter,i+1)))
        return show


    def updateFilter(self, widget):
        self.treemodelfilter = self.liststore.filter_new(root=None)
        self.treemodelfilter.set_visible_func(self.applyFilter)
        self.treemodelsorted = gtk.TreeModelSort(self.treemodelfilter)
        self.treeview.set_model(self.treemodelsorted)


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

                # Remove any old columns
                for c in self.treeview.get_columns():
                    self.treeview.remove_column(c)

                # Add columns and rows to filter table
                self.filtertable = self.wTree.get_widget("FilterTable")
                self.filtertable.resize(len(self.logfile.params()), 2)
                textrenderer = gtk.CellRendererText()
                col = 1
                self.filtercboxes = []
                for p in self.logfile.params():
                    column = gtk.TreeViewColumn(p, textrenderer, text=col)
                    column.set_sizing(gtk.TREE_VIEW_COLUMN_AUTOSIZE)
                    column.set_resizable(True)
                    column.set_clickable(True)
                    column.set_sort_column_id(col)
                    self.treeview.append_column(column)
                    label = gtk.Label(p)
                    label.set_justify(gtk.JUSTIFY_RIGHT)
                    label.show()
                    cbox = gtk.combo_box_new_text()
                    self.filtercboxes.append(cbox)
                    self.filtercboxes[-1].show()
                    self.filtercboxes[-1].connect("changed", self.updateFilter)
                    self.filtertable.attach(label, 0, 1, col-1, col)
                    self.filtertable.attach(cbox, 1, 2, col-1, col)
                    col = col + 1

                # Add data to table
                for n in self.logfile.networkdefs():
                    itt = self.liststore.append(n)

                # Add data to columns
                tmp = []
                for i in range(len(self.logfile.params())):
                    tmp.append([]) 

                for n in self.logfile.networkdefs():
                    for i in range(1,len(self.logfile.params())+1):
                        tmp[i-1].append(n[i])


                for i in range(len(tmp)):
                    tmp[i] = unique(tmp[i])
                    tmp[i].sort()
                    self.filtercboxes[i].append_text("All")
                    self.filtercboxes[i].set_active(0)
                    for n in tmp[i]:
                        self.filtercboxes[i].append_text(str(n))

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
