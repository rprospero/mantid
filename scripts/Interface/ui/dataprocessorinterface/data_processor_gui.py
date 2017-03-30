try:
    from mantidplot import *
except ImportError:
    canMantidPlot = False

from functools import partial
import ui_data_processor_window
from PyQt4 import QtGui, QtCore
from mantid.simpleapi import *
from mantidqtpython import MantidQt
from sans.sans_presenter import SANSPresenter

canMantidPlot = True


# ----------------------------------------------------------------------------------------------------------------------
# Free Functions
# ----------------------------------------------------------------------------------------------------------------------

def open_file_dialog(line_edit, filter_text):
    dlg = QtGui.QFileDialog()
    dlg.setFileMode(QtGui.QFileDialog.AnyFile)
    dlg.setFilter(filter_text)
    if dlg.exec_():
        file_names = dlg.selectedFiles()
        if file_names:
            line_edit.setText(file_names[0])


# ----------------------------------------------------------------------------------------------------------------------
# Gui Classes
# ----------------------------------------------------------------------------------------------------------------------

class MainPresenter(MantidQt.MantidWidgets.DataProcessorMainPresenter):
    """
    A DataProcessorMainPresenter. The base class provides default implementations
    but we should re-implement the following methods:
    - getPreprocessingOptionsAsString() -- to supply global pre-processing options to the table widget
    - getProcessingOptions() -- to supply global processing options
    - getPostprocessingOptions() -- to supply global post-processing options
    - notifyADSChanged() -- to act when the ADS changed, typically we want to update
      table actions with the list of table workspaces that can be loaded into the interface

    This is an intermediate layer needed in python. Ideally our gui class should
    inherit from 'DataProcessorMainPresenter' directly and provide the required implementations,
    but multiple inheritance does not seem to be fully supported, hence we need this extra class.
    """

    def __init__(self, gui):
        super(MantidQt.MantidWidgets.DataProcessorMainPresenter, self).__init__()
        self.gui = gui

    def getPreprocessingOptionsAsString(self):
        """
        Return global pre-processing options as a string.
        The string must be a sequence of key=value separated by ','.
        """
        return ""

    def getProcessingOptions(self):
        """
        Return global processing options as a string.
        The string must be a sequence of key=value separated by ','.
        """
        return ""

    def getPostprocessingOptions(self):
        """
        Return global post-processing options as a string.
        The string must be a sequence of key=value separated by ','.
        """
        return ""

    def notifyADSChanged(self, workspace_list):
        """
        The widget will call this method when something changes in the ADS.
        The argument is the list of table workspaces that can be loaded into
        the table. This method is intended to be used to update the table actions,
        specifically, to populate the 'Open Table' menu with the list of table
        workspaces.
        """
        self.gui.add_actions_to_menus(workspace_list)


class DataProcessorGui(QtGui.QMainWindow, ui_data_processor_window.Ui_dataProcessorWindow):

    data_processor_table = None
    main_presenter = None

    def __init__(self):
        """
        Initialise the interface
        """
        super(QtGui.QMainWindow, self).__init__()
        self.setupUi(self)
        self.sans_presenter = SANSPresenter(self)

    def setup_layout(self):
        """
        Do further setup that could not be done in the designer.
        So far only two menus have been added, we need to add the processing table manually.
        """

        # The white list (mandatory)
        # Defines the number of columns, their names and the algorithm properties linked to them
        # Metdho 'addElement' adds a column to the widget
        # The first argument is the name of the column
        # The second argument is the algorithm property
        # The third argument is a brief description of the column
        # The fourth argument is a boolean indicating if the value in this column will be used to name the reduced run
        # The fifth arument is a prefix added to the value in this column used to generate the name of the reduced run
        # (unused if the previous argument is false)
        # In addition to the specified columns, a last column 'Options' is always added
        whitelist = MantidQt.MantidWidgets.DataProcessorWhiteList()
        whitelist.addElement('SampleScatter', 'SampleScatter',
                             'The run number of the scatter sample', False, '')
        whitelist.addElement('SampleTransmission', 'SampleTransmission',
                             'The run number of the transmission sample', False, '')
        whitelist.addElement('SampleDirect', 'SampleDirect',
                             'The run number of the direct sample', False, '')
        whitelist.addElement('CanScatter', 'CanScatter',
                             'The run number of the scatter can', False, '')
        whitelist.addElement('CanTransmission', 'CanTransmission',
                             'The run number of the transmission can', False, '')
        whitelist.addElement('CanDirect', 'CanDirect',
                             'The run number of the direct can', False, '')

        # Processing algorithm (mandatory)
        # The main reduction algorithm
        # A number of prefixes equal to the number of output workspace properties must be specified
        # This prefixes are used in combination with the whitelist to name the reduced workspaces
        # For instance in this case, the first output workspace will be named IvsQ_binned_<runno>,
        # because column 'Run' is the only column used to create the workspace name, according to
        # the whitelist above
        # Additionally (not specified here) a blacklist of properties can be specified as the third
        # argument. These properties will not appear in the 'Options' column when typing
        alg = MantidQt.MantidWidgets.DataProcessorProcessingAlgorithm('ReflectometryReductionOneAuto','IvsQ_binned_, IvsQ_, IvsLam_','')

        # --------------------------------------------------------------------------------------------------------------
        # Main Tab
        # --------------------------------------------------------------------------------------------------------------
        # The table widget
        # A main presenter
        # Needed to supply global options for pre-processing/processing/post-processing to the widget
        self.data_processor_table = MantidQt.MantidWidgets.QDataProcessorWidget(whitelist, alg, self)
        self.main_presenter = MainPresenter(self)
        self._setup_main_tab()

        # The widget will emit a 'processButtonClicked' signal when 'Process' is selected
        self.data_processor_table.processButtonClicked.connect(self._process)

        # Set some values in the table
        self.data_processor_table.setCell("13460", 0, 0)
        self.data_processor_table.setCell("0.7", 0, 1)
        self.data_processor_table.setCell("13463,13464", 0, 2)
        self.data_processor_table.setCell("0.01", 0, 5)

        # Get values from the table
        print(self.data_processor_table.getCell(0, 0))
        print(self.data_processor_table.getCell(0, 1))
        print(self.data_processor_table.getCell(0, 2))
        print(self.data_processor_table.getCell(0, 5))

        return True

    def get_cell(self, row, column, convert_to=None):
        value = self.data_processor_table.getCell(row, column)
        return value if convert_to is None else convert_to(value)

    def _setup_main_tab(self):
        # --------------------------------------------------------------------------------------------------------------
        # Header setup
        # --------------------------------------------------------------------------------------------------------------
        self.user_file_button.clicked.connect(self._load_user_file)

        # --------------------------------------------------------------------------------------------------------------
        # Table setup
        # --------------------------------------------------------------------------------------------------------------
        # Add the presenter to the data processor
        self.data_processor_table.accept(self.main_presenter)

        # Set the list of available instruments in the widget and the default instrument
        self.data_processor_table.setInstrumentList('SANS2D, LOQ, LARMOR', 'SANS2D')

        # The widget will emit a 'runAsPythonScript' signal to run python code
        self.data_processor_table.runAsPythonScript.connect(self._run_python_code)

        self.layoutBase.addWidget(self.data_processor_table)

        # --------------------------------------------------------------------------------------------------------------
        # Footer setup
        # --------------------------------------------------------------------------------------------------------------

    def _load_user_file(self):
        open_file_dialog(self.user_file_line_edit, "User file (*.txt)")

    def _load_batch_file(self):
        open_file_dialog(self.batch_line_edit, "User file (*.txt)")

    def add_actions_to_menus(self, workspace_list):
        """
        Initialize table actions. Some table actions are not shown with the widget but they can be added to external menus.
        In this interface we have a 'File' menu and an 'Edit' menu
        """
        self.menuEdit.clear()
        self.menuFile.clear()

        # Actions that go in the 'Edit' menu
        self._create_action(MantidQt.MantidWidgets.DataProcessorProcessCommand(self.data_processor_table), self.menuEdit)
        self._create_action(MantidQt.MantidWidgets.DataProcessorPlotRowCommand(self.data_processor_table), self.menuEdit)
        self._create_action(MantidQt.MantidWidgets.DataProcessorAppendRowCommand(self.data_processor_table), self.menuEdit)
        self._create_action(MantidQt.MantidWidgets.DataProcessorCopySelectedCommand(self.data_processor_table), self.menuEdit)
        self._create_action(MantidQt.MantidWidgets.DataProcessorCutSelectedCommand(self.data_processor_table), self.menuEdit)
        self._create_action(MantidQt.MantidWidgets.DataProcessorPasteSelectedCommand(self.data_processor_table), self.menuEdit)
        self._create_action(MantidQt.MantidWidgets.DataProcessorClearSelectedCommand(self.data_processor_table), self.menuEdit)
        self._create_action(MantidQt.MantidWidgets.DataProcessorDeleteRowCommand(self.data_processor_table), self.menuEdit)

        # Actions that go in the 'File' menu
        self._create_action(MantidQt.MantidWidgets.DataProcessorOpenTableCommand(self.data_processor_table), self.menuFile, workspace_list)
        self._create_action(MantidQt.MantidWidgets.DataProcessorNewTableCommand(self.data_processor_table), self.menuFile)
        self._create_action(MantidQt.MantidWidgets.DataProcessorSaveTableCommand(self.data_processor_table), self.menuFile)
        self._create_action(MantidQt.MantidWidgets.DataProcessorSaveTableAsCommand(self.data_processor_table), self.menuFile)
        self._create_action(MantidQt.MantidWidgets.DataProcessorImportTableCommand(self.data_processor_table), self.menuFile)
        self._create_action(MantidQt.MantidWidgets.DataProcessorExportTableCommand(self.data_processor_table), self.menuFile)
        self._create_action(MantidQt.MantidWidgets.DataProcessorOptionsCommand(self.data_processor_table), self.menuFile)

    def _create_action(self, command, menu, workspace_list = None):
        """
        Create an action from a given DataProcessorCommand and add it to a given menu
        A 'workspace_list' can be provided but it is only intended to be used with DataProcessorOpenTableCommand.
        It refers to the list of table workspaces in the ADS that could be loaded into the widget. Note that only
        table workspaces with an appropriate number of columns and column types can be loaded.
        """
        if (workspace_list is not None and command.name() == "Open Table"):
            submenu = QtGui.QMenu(command.name(), self)
            submenu.setIcon(QtGui.QIcon(command.icon()))

            for ws in workspace_list:
                ws_command = MantidQt.MantidWidgets.DataProcessorWorkspaceCommand(self.data_processor_table, ws)
                action = QtGui.QAction(QtGui.QIcon(ws_command.icon()), ws_command.name(), self)
                action.triggered.connect(lambda: self._connect_action(ws_command))
                submenu.addAction(action)

            menu.addMenu(submenu)
        else:
            action = QtGui.QAction(QtGui.QIcon(command.icon()), command.name(), self)
            action.setShortcut(command.shortcut())
            action.setStatusTip(command.tooltip())
            action.triggered.connect(lambda: self._connect_action(command))
            menu.addAction(action)

    def _connect_action(self, command):
        """
        Executes an action
        """
        command.execute()

    def _run_python_code(self, text):
        """
        Re-emits 'runPytonScript' signal
        """
        mantidplot.runPythonScript(text, True)

    def _process(self):
        """
        Process runs
        """
        print "Custom processing of runs"
