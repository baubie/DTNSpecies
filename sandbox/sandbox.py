#!/usr/bin/python

import sys
import pygtk
import gtk
import gtk.glade
from collections import defaultdict
from LogFile import LogFile
from numpy import mean, std, array, zeros

import matplotlib
matplotlib.use('GTK')   
from matplotlib.figure import Figure   
from matplotlib.axes import Subplot   
from matplotlib.backends.backend_gtk import FigureCanvasGTK, NavigationToolbar   
from matplotlib.image import NonUniformImage

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
            
            "on_btnPlotAnalysis_clicked": self.plotAnalysis,
            "on_checkShowTemplate_clicked": self.showTemplate,
            "on_btnClearFilter_clicked": self.clearFilter,
            "on_tbClear_clicked": self.clearPlot
        }

        self.wTree.signal_autoconnect(dic)

        # Create a LogFile object to handle data
        self.logfile = LogFile()

        # Create an empty plot window for tuning curves
        self.figureDT = Figure(figsize=(6,4), dpi=72)   
        self.axisDT = self.figureDT.add_subplot(111)   
        self.axisDT.set_xlabel('Duration')   
        self.axisDT.set_ylabel('Mean Number of Spikes')   
        self.axisDT.grid(True)   
        self.canvasDT = FigureCanvasGTK(self.figureDT) # a gtk.DrawingArea   
        self.canvasDT.show()   
        self.graphviewDT = self.wTree.get_widget("vboxTuning")   
        self.graphviewDT.pack_end(self.canvasDT)
        self.maxSpikes = 1
        self.showTemplate = False

        # Create an empty plot window for analysis
        self.figureAN = Figure(dpi=72)
        self.axisAN = self.figureAN.add_subplot(111)
        self.canvasAN = FigureCanvasGTK(self.figureAN)
        self.canvasAN.show()
        self.graphviewAN = self.wTree.get_widget("vboxAnalysis")
        self.graphviewAN.pack_end(self.canvasAN)

        # Setup the analyze window
        self.cbXAxis = gtk.combo_box_new_text()
        self.cbYAxis = gtk.combo_box_new_text()
        self.cbZAxis = gtk.combo_box_new_text()
        self.cbXAxis.show()
        self.cbYAxis.show()
        self.cbZAxis.show()
        self.hboxAnalyze = self.wTree.get_widget("hboxAnalyze")
        labelX = gtk.Label("X-Axis")
        labelX.show()
        labelY = gtk.Label("Y-Axis")
        labelY.show()
        labelZ = gtk.Label("Z-Axis")
        labelZ.show()
        self.hboxAnalyze.pack_start(labelX)
        self.hboxAnalyze.pack_start(self.cbXAxis)
        self.hboxAnalyze.pack_start(labelY)
        self.hboxAnalyze.pack_start(self.cbYAxis)
        self.hboxAnalyze.pack_start(labelZ)
        self.hboxAnalyze.pack_start(self.cbZAxis)

    def plotAnalysis(self, widget):
        Xvar = self.cbXAxis.get_active_text()
        Yvar = self.cbYAxis.get_active_text()
        Zvar = self.cbZAxis.get_active_text()
        if Xvar == None or Yvar == None or Zvar == None:
            return


        if Zvar == "None":
            XvarIndex = self.logfile.params().index(Xvar)+1
            YvarIndex = self.logfile.params().index(Yvar)+1
            rowiter = self.treemodelsorted.get_iter_first()
            values = defaultdict(list)

            while rowiter != None:
                X = self.treemodelsorted.get_value(rowiter,XvarIndex)
                Y = self.treemodelsorted.get_value(rowiter,YvarIndex)
                values[float(X)].append(float(Y))
                rowiter = self.treemodelsorted.iter_next(rowiter)

            X = []
            Y = []
            for k in sorted(values.keys()):
                X.append(k)
                Y.append(mean(values[k]))

            self.axisAN.cla()        
            self.figureAN.clf()
            self.axisAN = self.figureAN.add_subplot(111)
            self.axisAN.plot(X,Y, 'k', linewidth=4)
            self.axisAN.set_xlabel(Xvar)   
            self.axisAN.set_ylabel(Yvar)   
            self.canvasAN.draw()

        else:
            XvarIndex = self.logfile.params().index(Xvar)+1
            YvarIndex = self.logfile.params().index(Yvar)+1
            ZvarIndex = self.logfile.params().index(Zvar)+1
            rowiter = self.treemodelsorted.get_iter_first()
            values = {}
            Ykeys = []

            while rowiter != None:
                X = self.treemodelsorted.get_value(rowiter,XvarIndex)
                Y = self.treemodelsorted.get_value(rowiter,YvarIndex)
                Z = self.treemodelsorted.get_value(rowiter,ZvarIndex)
                Ykeys.append(Y)
                values.setdefault(X,defaultdict(list))[Y].append(Z)
                rowiter = self.treemodelsorted.iter_next(rowiter)


            Ykeys = unique(Ykeys)
            XY = []
            for k in sorted(values.keys()):
                tmp = []
                for k2 in sorted(Ykeys):
                    if values[k].has_key(k2):
                        tmp.append(mean(values[k][k2]))
                    else:
                        tmp.append(0)
                XY.append(tmp)
            
            Z = array(XY)

            self.axisAN.cla()        
            self.figureAN.clf()
            self.axisAN = self.figureAN.add_subplot(111)
            im = NonUniformImage(self.axisAN, interpolation='nearest', extent=(min(values.keys()),max(values.keys()),min(Ykeys),max(Ykeys)))
            
            im.set_data(values.keys(), Ykeys, Z.transpose())
            self.axisAN.images.append(im)
            self.axisAN.set_xlim(min(values.keys()),max(values.keys()))
            self.axisAN.set_ylim(min(Ykeys),max(Ykeys))
            self.axisAN.set_xlabel(Xvar)   
            self.axisAN.set_ylabel(Yvar)   
            self.axisAN.set_title(Zvar)   
            self.figureAN.colorbar(im)
            self.canvasAN.draw()



    def showTemplate(self, widget):
        self.showTemplate = widget.get_active()

    def plotRows(self, widget, path, column):
        (model, pathlist) = self.treeview.get_selection().get_selected_rows()
        for p in pathlist:
            treeiter = self.treemodelsorted.get_iter(p)
            X = self.logfile.getdurs()
            Y = self.logfile.getresults(self.treemodelsorted.get_value(treeiter,0))
            tmp = Y[:]
            tmp.append(self.maxSpikes)
            self.maxSpikes = max(tmp)
            self.axisDT.plot(X, Y, linewidth=2)
            self.axisDT.set_ylim( (0, self.maxSpikes+0.1) )

        if self.showTemplate:
            Y = self.logfile.getresults(0)
            tmp = Y[:]
            tmp.append(self.maxSpikes)
            self.maxSpikes = max(tmp)
            self.axisDT.plot(X, Y, 'k', linewidth=3)
            self.axisDT.set_ylim( (0, self.maxSpikes+0.1) )


        self.canvasDT.draw()

    def clearPlot(self, widget):
        self.maxSpikes = 1
        self.axisDT.cla()
        self.axisDT.set_xlabel('Duration')   
        self.axisDT.set_ylabel('Mean Number of Spikes')   
        self.axisDT.grid(True)   
        self.canvasDT.draw()

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

                self.cbXAxis.get_model().clear()
                self.cbYAxis.get_model().clear()
                self.cbZAxis.get_model().clear()
                self.cbZAxis.append_text("None")

                # Add columns to filter table and fill analysis dropdowns
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
                    self.cbXAxis.append_text(p)
                    self.cbYAxis.append_text(p)
                    self.cbZAxis.append_text(p)
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

                # Setup the filtered sorted liststores
                self.treemodelfilter = self.liststore.filter_new(root=None)
                self.treemodelfilter.set_visible_func(self.applyFilter)
                self.treemodelsorted = gtk.TreeModelSort(self.treemodelfilter)
                self.treeview.set_model(self.treemodelsorted)

        # Destroy the file chooser
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
