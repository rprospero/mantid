################################################################################
# Rules for producing the MantidUiWidgetsCore libary
################################################################################
# We need additional Qt libraries here
find_package ( Qt4 REQUIRED QtHelp QtWebKit QtNetwork QUIET )
include(${QT_USE_FILE})

find_package ( QScintilla REQUIRED )
include_directories ( ${QSCINTILLA_INCLUDE_DIR} )
add_definitions ( -DQSCINTILLA_DLL )

if(MAKE_VATES AND ParaView_FOUND)
  include( ${PARAVIEW_USE_FILE} )
endif()

# Python includes
include_directories ( ${PYTHON_INCLUDE_PATH})
if(MAKE_VATES AND ParaView_FOUND)
  include_directories (SYSTEM ${PARAVIEW_INCLUDE_DIRS})
endif()

set ( SRC_FILES
	src/AlgorithmDialog.cpp
	src/AlgorithmInputHistory.cpp
	src/AlgorithmPropertiesWidget.cpp
	src/AlgorithmRunner.cpp
	src/BatchAlgorithmRunner.cpp
	src/BoolPropertyWidget.cpp
	src/FileDialogHandler.cpp
	src/FilePropertyWidget.cpp
	src/GenericDialog.cpp
	src/HelpWindow.cpp
	src/FlowLayout.cpp
	src/InterfaceFactory.cpp
	src/InterfaceManager.cpp
	src/ListPropertyWidget.cpp
	src/ManageUserDirectories.cpp
	src/MantidColorMap.cpp
	src/MantidDesktopServices.cpp
	src/MantidDialog.cpp
	src/MantidHelpInterface.cpp
	src/MantidQwtIMDWorkspaceData.cpp
	src/MantidQwtWorkspaceData.cpp
	src/MantidWidget.cpp
	src/MdConstants.cpp
	src/MdPlottingCmapsProvider.cpp
	src/MdSettings.cpp
	src/Message.cpp
	src/NonOrthogonal.cpp
	src/MWRunFiles.cpp
	src/OptionsPropertyWidget.cpp
	src/pixmaps.cpp
	src/PlotAxis.cpp
	src/PowerScaleEngine.cpp
	src/PropertyWidget.cpp
	src/PropertyWidgetFactory.cpp
	src/PythonRunner.cpp
	src/PythonThreading.cpp
	src/QScienceSpinBox.cpp
	src/QtSignalChannel.cpp
	src/QwtRasterDataMD.cpp
	src/QwtRasterDataMDNonOrthogonal.cpp
	src/QwtWorkspaceBinData.cpp
	src/QwtWorkspaceSpectrumData.cpp
	src/RepoModel.cpp
	src/ScaleEngine.cpp
	src/ScriptRepositoryView.cpp
	src/SelectionNotificationService.cpp
	src/SignalBlocker.cpp
	src/SignalRange.cpp
	src/SyncedCheckboxes.cpp
	src/TextPropertyWidget.cpp
	src/TSVSerialiser.cpp
	src/UserSubWindow.cpp
	src/VatesViewerInterface.cpp
	src/WidgetScrollbarDecorator.cpp
        src/WindowIcons.cpp
        src/WorkspaceObserver.cpp
	src/WorkspaceIcons.cpp
  src/AlgorithmSelectorWidget.cpp
  src/CatalogHelper.cpp
  src/CatalogSearch.cpp
  src/CatalogSelector.cpp
  src/CheckboxHeader.cpp
  src/ColorBarWidget.cpp
  src/DataProcessorUI/DataProcessorGenerateNotebook.cpp
  src/DataProcessorUI/DataProcessorOneLevelTreeManager.cpp
  src/DataProcessorUI/DataProcessorPostprocessingAlgorithm.cpp
  src/DataProcessorUI/DataProcessorPreprocessingAlgorithm.cpp
  src/DataProcessorUI/DataProcessorProcessingAlgorithm.cpp
  src/DataProcessorUI/DataProcessorProcessingAlgorithmBase.cpp
  src/DataProcessorUI/DataProcessorTwoLevelTreeManager.cpp
  src/DataProcessorUI/DataProcessorWhiteList.cpp
  src/DataProcessorUI/GenericDataProcessorPresenter.cpp
  src/DataProcessorUI/ParseKeyValueString.cpp
  src/DataProcessorUI/QDataProcessorOneLevelTreeModel.cpp
  src/DataProcessorUI/QDataProcessorTwoLevelTreeModel.cpp
  src/DataProcessorUI/QDataProcessorWidget.cpp
  src/DataProcessorUI/QtDataProcessorOptionsDialog.cpp
  src/DataSelector.cpp
  src/DiagResults.cpp
  src/DisplayCurveFit.cpp
  src/DoubleDialogEditor.cpp
  src/DoubleSpinBox.cpp
  src/ErrorCurve.cpp
  src/FilenameDialogEditor.cpp
  src/FindDialog.cpp
  src/FindReplaceDialog.cpp
  src/FitOptionsBrowser.cpp
  src/FitPropertyBrowser.cpp
  src/FormulaDialogEditor.cpp
  src/FunctionBrowser.cpp
  src/HintingLineEdit.cpp
  src/IndirectInstrumentConfig.cpp
  src/InputController.cpp
  src/InstrumentSelector.cpp
  src/InstrumentView/BinDialog.cpp
  src/InstrumentView/CollapsiblePanel.cpp
  src/InstrumentView/ColorMapWidget.cpp
  src/InstrumentView/CompAssemblyActor.cpp
  src/InstrumentView/ComponentActor.cpp
  src/InstrumentView/DetXMLFile.cpp
  src/InstrumentView/GLActor.cpp
  src/InstrumentView/GLActorCollection.cpp
  src/InstrumentView/GLActorVisitor.cpp
  src/InstrumentView/GLColor.cpp
  src/InstrumentView/GLObject.cpp
  src/InstrumentView/ICompAssemblyActor.cpp
  src/InstrumentView/InstrumentActor.cpp
  src/InstrumentView/InstrumentTreeModel.cpp
  src/InstrumentView/InstrumentTreeWidget.cpp
  src/InstrumentView/InstrumentWidget.cpp
  src/InstrumentView/InstrumentWidgetMaskTab.cpp
  src/InstrumentView/InstrumentWidgetPickTab.cpp
  src/InstrumentView/InstrumentWidgetRenderTab.cpp
  src/InstrumentView/InstrumentWidgetTab.cpp
  src/InstrumentView/InstrumentWidgetTreeTab.cpp
  src/InstrumentView/MantidGLWidget.cpp
  src/InstrumentView/MaskBinsData.cpp
  src/InstrumentView/ObjCompAssemblyActor.cpp
  src/InstrumentView/ObjComponentActor.cpp
  src/InstrumentView/OneCurvePlot.cpp
  src/InstrumentView/OpenGLError.cpp
  src/InstrumentView/PanelsSurface.cpp
  src/InstrumentView/PeakMarker2D.cpp
  src/InstrumentView/PeakOverlay.cpp
  src/InstrumentView/Projection3D.cpp
  src/InstrumentView/ProjectionSurface.cpp
  src/InstrumentView/RectF.cpp
  src/InstrumentView/RectangularDetectorActor.cpp
  src/InstrumentView/RotationSurface.cpp
  src/InstrumentView/SampleActor.cpp
  src/InstrumentView/Shape2D.cpp
  src/InstrumentView/Shape2DCollection.cpp
  src/InstrumentView/SimpleWidget.cpp
  src/InstrumentView/StructuredDetectorActor.cpp
  src/InstrumentView/UCorrectionDialog.cpp
  src/InstrumentView/UnwrappedCylinder.cpp
  src/InstrumentView/UnwrappedSphere.cpp
  src/InstrumentView/UnwrappedSurface.cpp
  src/InstrumentView/Viewport.cpp
  src/InstrumentView/XIntegrationControl.cpp
  src/LineEditWithClear.cpp
  src/LogValueSelector.cpp
  src/MWDiag.cpp
  src/MWView.cpp
  src/MantidHelpWindow.cpp
  src/MantidTreeWidget.cpp
  src/MantidTreeWidgetItem.cpp
  src/MantidWSIndexDialog.cpp
  src/MessageDisplay.cpp
  src/MultifitSetupDialog.cpp
  src/MuonFitDataSelector.cpp
  src/MuonFitPropertyBrowser.cpp
  src/MuonFunctionBrowser.cpp
  src/PeakPicker.cpp
  src/PeriodicTableWidget.cpp
  src/PreviewPlot.cpp
  src/ProcessingAlgoWidget.cpp
  src/ProjectSavePresenter.cpp
  src/ProjectSaveModel.cpp
  src/PropertyHandler.cpp
  src/RangeSelector.cpp
  src/RenameParDialog.cpp
  src/SafeQwtPlot.cpp
  src/SaveWorkspaces.cpp
  src/ScriptEditor.cpp
  src/SelectFunctionDialog.cpp
  src/SelectWorkspacesDialog.cpp
  src/SequentialFitDialog.cpp
  src/SlicingAlgorithmDialog.cpp
  src/SlitCalculator.cpp
  src/StringDialogEditor.cpp
  src/StringEditorFactory.cpp
  src/TrackedAction.cpp
  src/UserFunctionDialog.cpp
  src/WorkspaceEditorFactory.cpp
  src/WorkspacePresenter/ADSAdapter.cpp
  src/WorkspacePresenter/QWorkspaceDockView.cpp
  src/WorkspacePresenter/WorkspacePresenter.cpp
  src/WorkspaceSelector.cpp
  src/pqHelpWindow.cxx
  src/pythonCalc.cpp
)

# Header files with Q_OBJECT that qmake will "moc"
set ( MOC_FILES
  inc/MantidUiWidgetsCore/AlgorithmDialog.h
  inc/MantidUiWidgetsCore/AlgorithmRunner.h
  inc/MantidUiWidgetsCore/AlgorithmPropertiesWidget.h
  inc/MantidUiWidgetsCore/BatchAlgorithmRunner.h
  inc/MantidUiWidgetsCore/BoolPropertyWidget.h
  inc/MantidUiWidgetsCore/FilePropertyWidget.h
  inc/MantidUiWidgetsCore/GenericDialog.h
  inc/MantidUiWidgetsCore/ListPropertyWidget.h
  inc/MantidUiWidgetsCore/ManageUserDirectories.h
  inc/MantidUiWidgetsCore/MantidDialog.h
  inc/MantidUiWidgetsCore/MantidHelpInterface.h
  inc/MantidUiWidgetsCore/MantidWidget.h
  inc/MantidUiWidgetsCore/MWRunFiles.h
  inc/MantidUiWidgetsCore/OptionsPropertyWidget.h
  inc/MantidUiWidgetsCore/PropertyWidget.h
  inc/MantidUiWidgetsCore/PythonRunner.h
  inc/MantidUiWidgetsCore/QScienceSpinBox.h
  inc/MantidUiWidgetsCore/QtSignalChannel.h
  inc/MantidUiWidgetsCore/ScriptRepositoryView.h
  inc/MantidUiWidgetsCore/RepoTreeView.h
  inc/MantidUiWidgetsCore/RepoModel.h
  inc/MantidUiWidgetsCore/SyncedCheckboxes.h
  inc/MantidUiWidgetsCore/TextPropertyWidget.h
  inc/MantidUiWidgetsCore/UserSubWindow.h
  inc/MantidUiWidgetsCore/VatesViewerInterface.h
  inc/MantidUiWidgetsCore/WorkspaceObserver.h
  inc/MantidUiWidgetsCore/SelectionNotificationService.h

  inc/MantidUiWidgetsCore/AlgorithmSelectorWidget.h
  inc/MantidUiWidgetsCore/CheckboxHeader.h
  inc/MantidUiWidgetsCore/ColorBarWidget.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorCommandAdapter.h
  inc/MantidUiWidgetsCore/DataProcessorUI/QDataProcessorOneLevelTreeModel.h
  inc/MantidUiWidgetsCore/DataProcessorUI/QDataProcessorTwoLevelTreeModel.h
  inc/MantidUiWidgetsCore/DataProcessorUI/QDataProcessorWidget.h
  inc/MantidUiWidgetsCore/DataProcessorUI/QtDataProcessorOptionsDialog.h
  inc/MantidUiWidgetsCore/DataSelector.h
  inc/MantidUiWidgetsCore/DiagResults.h
  inc/MantidUiWidgetsCore/DisplayCurveFit.h
  inc/MantidUiWidgetsCore/DoubleDialogEditor.h
  inc/MantidUiWidgetsCore/DoubleSpinBox.h
  inc/MantidUiWidgetsCore/FilenameDialogEditor.h
  inc/MantidUiWidgetsCore/FormulaDialogEditor.h
  inc/MantidUiWidgetsCore/FindReplaceDialog.h
  inc/MantidUiWidgetsCore/FindDialog.h
  inc/MantidUiWidgetsCore/FitPropertyBrowser.h
  inc/MantidUiWidgetsCore/FitOptionsBrowser.h
  inc/MantidUiWidgetsCore/FunctionBrowser.h
  inc/MantidUiWidgetsCore/HintingLineEdit.h
  inc/MantidUiWidgetsCore/CatalogSearch.h
  inc/MantidUiWidgetsCore/CatalogSelector.h
  inc/MantidUiWidgetsCore/InstrumentSelector.h
  inc/MantidUiWidgetsCore/IndirectInstrumentConfig.h
  inc/MantidUiWidgetsCore/InputController.h
  inc/MantidUiWidgetsCore/MantidWSIndexDialog.h
  inc/MantidUiWidgetsCore/MantidTreeWidget.h
  inc/MantidUiWidgetsCore/MantidHelpWindow.h
  inc/MantidUiWidgetsCore/MessageDisplay.h
  inc/MantidUiWidgetsCore/MultifitSetupDialog.h
  inc/MantidUiWidgetsCore/MuonFitDataSelector.h
  inc/MantidUiWidgetsCore/MuonFitPropertyBrowser.h
  inc/MantidUiWidgetsCore/MuonFunctionBrowser.h
  inc/MantidUiWidgetsCore/MWDiag.h
  inc/MantidUiWidgetsCore/MWView.h
  inc/MantidUiWidgetsCore/PeakPicker.h
  inc/MantidUiWidgetsCore/PeriodicTableWidget.h
  inc/MantidUiWidgetsCore/pqHelpWindow.h
  inc/MantidUiWidgetsCore/PreviewPlot.h
  inc/MantidUiWidgetsCore/PropertyHandler.h
  inc/MantidUiWidgetsCore/ProcessingAlgoWidget.h
  inc/MantidUiWidgetsCore/pythonCalc.h
  inc/MantidUiWidgetsCore/RangeSelector.h
  inc/MantidUiWidgetsCore/RenameParDialog.h
  inc/MantidUiWidgetsCore/SafeQwtPlot.h
  inc/MantidUiWidgetsCore/SaveWorkspaces.h
  inc/MantidUiWidgetsCore/ScriptEditor.h
  inc/MantidUiWidgetsCore/SelectFunctionDialog.h
  inc/MantidUiWidgetsCore/SelectWorkspacesDialog.h
  inc/MantidUiWidgetsCore/SequentialFitDialog.h
  inc/MantidUiWidgetsCore/SlicingAlgorithmDialog.h
  inc/MantidUiWidgetsCore/SlitCalculator.h
  inc/MantidUiWidgetsCore/StringDialogEditor.h
  inc/MantidUiWidgetsCore/StringEditorFactory.h
  inc/MantidUiWidgetsCore/TrackedAction.h
  inc/MantidUiWidgetsCore/UserFunctionDialog.h
  inc/MantidUiWidgetsCore/WorkspaceEditorFactory.h
  inc/MantidUiWidgetsCore/WorkspaceSelector.h
  inc/MantidUiWidgetsCore/LineEditWithClear.h
  inc/MantidUiWidgetsCore/InstrumentView/BinDialog.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentTreeModel.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentTreeWidget.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidget.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetTab.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetRenderTab.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetPickTab.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetMaskTab.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetTreeTab.h
  inc/MantidUiWidgetsCore/InstrumentView/ColorMapWidget.h
  inc/MantidUiWidgetsCore/InstrumentView/MantidGLWidget.h
  inc/MantidUiWidgetsCore/InstrumentView/OneCurvePlot.h
  inc/MantidUiWidgetsCore/InstrumentView/CollapsiblePanel.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentActor.h
  inc/MantidUiWidgetsCore/InstrumentView/PeakOverlay.h
  inc/MantidUiWidgetsCore/InstrumentView/ProjectionSurface.h
  inc/MantidUiWidgetsCore/InstrumentView/Projection3D.h
  inc/MantidUiWidgetsCore/InstrumentView/Shape2DCollection.h
  inc/MantidUiWidgetsCore/InstrumentView/UnwrappedSurface.h
  inc/MantidUiWidgetsCore/InstrumentView/UCorrectionDialog.h
  inc/MantidUiWidgetsCore/InstrumentView/XIntegrationControl.h
  inc/MantidUiWidgetsCore/LogValueSelector.h
  inc/MantidUiWidgetsCore/WorkspacePresenter/QWorkspaceDockView.h
)

# Add the include files are NOT already in MOC_FILES
set ( INC_FILES
  ${MOC_FILES}
	inc/MantidUiWidgetsCore/AlgorithmInputHistory.h
	inc/MantidUiWidgetsCore/AlgorithmRunner.h
	inc/MantidUiWidgetsCore/BatchAlgorithmRunner.h
	inc/MantidUiWidgetsCore/DllOption.h
	inc/MantidUiWidgetsCore/FileDialogHandler.h
	inc/MantidUiWidgetsCore/FlowLayout.h
	inc/MantidUiWidgetsCore/GraphOptions.h
	inc/MantidUiWidgetsCore/DistributionOptions.h
	inc/MantidUiWidgetsCore/HelpWindow.h
	inc/MantidUiWidgetsCore/InterfaceFactory.h
	inc/MantidUiWidgetsCore/InterfaceManager.h
	inc/MantidUiWidgetsCore/IProjectSerialisable.h
	inc/MantidUiWidgetsCore/MantidColorMap.h
	inc/MantidUiWidgetsCore/MantidDesktopServices.h
	inc/MantidUiWidgetsCore/MantidQwtIMDWorkspaceData.h
	inc/MantidUiWidgetsCore/MantidQwtWorkspaceData.h
	inc/MantidUiWidgetsCore/MantidAlgorithmMetatype.h
	inc/MantidUiWidgetsCore/MdConstants.h
	inc/MantidUiWidgetsCore/MdPlottingCmapsProvider.h
	inc/MantidUiWidgetsCore/MdSettings.h
	inc/MantidUiWidgetsCore/NonOrthogonal.h
	inc/MantidUiWidgetsCore/pixmaps.h
	inc/MantidUiWidgetsCore/PlotAxis.h
	inc/MantidUiWidgetsCore/PowerScaleEngine.h
	inc/MantidUiWidgetsCore/PropertyWidgetFactory.h
	inc/MantidUiWidgetsCore/PythonSystemHeader.h
	inc/MantidUiWidgetsCore/PythonThreading.h
	inc/MantidUiWidgetsCore/QScienceSpinBox.h
	inc/MantidUiWidgetsCore/QwtRasterDataMD.h
	inc/MantidUiWidgetsCore/QwtRasterDataMDNonOrthogonal.h
	inc/MantidUiWidgetsCore/QwtWorkspaceBinData.h
	inc/MantidUiWidgetsCore/QwtWorkspaceSpectrumData.h
	inc/MantidUiWidgetsCore/ScaleEngine.h
	inc/MantidUiWidgetsCore/ScriptRepositoryView.h
	inc/MantidUiWidgetsCore/SelectionNotificationService.h
	inc/MantidUiWidgetsCore/SignalBlocker.h
	inc/MantidUiWidgetsCore/SignalRange.h
	inc/MantidUiWidgetsCore/TSVSerialiser.h
	inc/MantidUiWidgetsCore/qwt_compat.h
	inc/MantidUiWidgetsCore/WidgetScrollbarDecorator.h
	inc/MantidUiWidgetsCore/WindowIcons.h
	inc/MantidUiWidgetsCore/WorkspaceIcons.h

  inc/MantidUiWidgetsCore/AlgorithmHintStrategy.h
  inc/MantidUiWidgetsCore/CatalogHelper.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorAppendGroupCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorAppendRowCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorClearSelectedCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorCommandBase.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorCollapseGroupsCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorCopySelectedCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorCutSelectedCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorDeleteGroupCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorDeleteRowCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorExpandCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorExpandGroupsCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorExportTableCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorGenerateNotebook.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorGroupRowsCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorImportTableCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorMainPresenter.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorNewTableCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorOneLevelTreeManager.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorOpenTableCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorOptionsCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorPasteSelectedCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorPlotGroupCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorPlotRowCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorPostprocessingAlgorithm.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorPreprocessingAlgorithm.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorPresenter.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorProcessCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorProcessingAlgorithm.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorProcessingAlgorithmBase.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorSaveTableAsCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorSaveTableCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorTreeManager.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorTwoLevelTreeManager.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorVectorString.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorView.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorWhiteList.h
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorWorkspaceCommand.h
  inc/MantidUiWidgetsCore/DataProcessorUI/GenericDataProcessorPresenter.h
  inc/MantidUiWidgetsCore/DataProcessorUI/GenericDataProcessorPresenterFactory.h
  inc/MantidUiWidgetsCore/DataProcessorUI/ParseKeyValueString.h
  inc/MantidUiWidgetsCore/ErrorCurve.h
  inc/MantidUiWidgetsCore/HintStrategy.h
  inc/MantidUiWidgetsCore/IFunctionBrowser.h
  inc/MantidUiWidgetsCore/IMuonFitDataModel.h
  inc/MantidUiWidgetsCore/IMuonFitDataSelector.h
  inc/MantidUiWidgetsCore/IMuonFitFunctionModel.h
  inc/MantidUiWidgetsCore/IProjectSaveView.h
  inc/MantidUiWidgetsCore/IWorkspaceFitControl.h
  inc/MantidUiWidgetsCore/InstrumentView/BinDialog.h
  inc/MantidUiWidgetsCore/InstrumentView/CollapsiblePanel.h
  inc/MantidUiWidgetsCore/InstrumentView/ColorMapWidget.h
  inc/MantidUiWidgetsCore/InstrumentView/CompAssemblyActor.h
  inc/MantidUiWidgetsCore/InstrumentView/ComponentActor.h
  inc/MantidUiWidgetsCore/InstrumentView/DetXMLFile.h
  inc/MantidUiWidgetsCore/InstrumentView/GLActor.h
  inc/MantidUiWidgetsCore/InstrumentView/GLActorCollection.h
  inc/MantidUiWidgetsCore/InstrumentView/GLActorVisitor.h
  inc/MantidUiWidgetsCore/InstrumentView/GLColor.h
  inc/MantidUiWidgetsCore/InstrumentView/GLObject.h
  inc/MantidUiWidgetsCore/InstrumentView/ICompAssemblyActor.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentActor.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentTreeModel.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentTreeWidget.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidget.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetMaskTab.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetPickTab.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetRenderTab.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetTab.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetTreeTab.h
  inc/MantidUiWidgetsCore/InstrumentView/InstrumentWidgetTypes.h
  inc/MantidUiWidgetsCore/InstrumentView/MantidGLWidget.h
  inc/MantidUiWidgetsCore/InstrumentView/MaskBinsData.h
  inc/MantidUiWidgetsCore/InstrumentView/ObjCompAssemblyActor.h
  inc/MantidUiWidgetsCore/InstrumentView/ObjComponentActor.h
  inc/MantidUiWidgetsCore/InstrumentView/OneCurvePlot.h
  inc/MantidUiWidgetsCore/InstrumentView/OpenGLError.h
  inc/MantidUiWidgetsCore/InstrumentView/PanelsSurface.h
  inc/MantidUiWidgetsCore/InstrumentView/PeakMarker2D.h
  inc/MantidUiWidgetsCore/InstrumentView/PeakOverlay.h
  inc/MantidUiWidgetsCore/InstrumentView/Projection3D.h
  inc/MantidUiWidgetsCore/InstrumentView/ProjectionSurface.h
  inc/MantidUiWidgetsCore/InstrumentView/RectF.h
  inc/MantidUiWidgetsCore/InstrumentView/RectangularDetectorActor.h
  inc/MantidUiWidgetsCore/InstrumentView/RotationSurface.h
  inc/MantidUiWidgetsCore/InstrumentView/SampleActor.h
  inc/MantidUiWidgetsCore/InstrumentView/Shape2D.h
  inc/MantidUiWidgetsCore/InstrumentView/Shape2DCollection.h
  inc/MantidUiWidgetsCore/InstrumentView/SimpleWidget.h
  inc/MantidUiWidgetsCore/InstrumentView/StructuredDetectorActor.h
  inc/MantidUiWidgetsCore/InstrumentView/UCorrectionDialog.h
  inc/MantidUiWidgetsCore/InstrumentView/UnwrappedCylinder.h
  inc/MantidUiWidgetsCore/InstrumentView/UnwrappedSphere.h
  inc/MantidUiWidgetsCore/InstrumentView/UnwrappedSurface.h
  inc/MantidUiWidgetsCore/InstrumentView/Viewport.h
  inc/MantidUiWidgetsCore/InstrumentView/XIntegrationControl.h
  inc/MantidUiWidgetsCore/LogValueSelector.h
  inc/MantidUiWidgetsCore/MantidDisplayBase.h
  inc/MantidUiWidgetsCore/MantidTreeWidgetItem.h
  inc/MantidUiWidgetsCore/MuonFitDataSelector.h
  inc/MantidUiWidgetsCore/MuonFunctionBrowser.h
  inc/MantidUiWidgetsCore/ProgressableView.h
  inc/MantidUiWidgetsCore/ProgressPresenter.h
  inc/MantidUiWidgetsCore/ProjectSavePresenter.h
  inc/MantidUiWidgetsCore/ProjectSaveModel.h
  inc/MantidUiWidgetsCore/ProgressableView.h
  inc/MantidUiWidgetsCore/WorkspacePresenter/ADSAdapter.h
  inc/MantidUiWidgetsCore/WorkspacePresenter/IWorkspaceDockView.h
  inc/MantidUiWidgetsCore/WorkspacePresenter/ViewNotifiable.h
  inc/MantidUiWidgetsCore/WorkspacePresenter/WorkspacePresenter.h
  inc/MantidUiWidgetsCore/WorkspacePresenter/WorkspaceProvider.h
  inc/MantidUiWidgetsCore/WorkspacePresenter/WorkspaceProviderNotifiable.h
  inc/MantidUiWidgetsCore/WorkspacePresenter/ADSAdapter.h
  inc/MantidUiWidgetsCore/WorkspacePresenter/IWorkspaceDockView.h
)

# QtDesigner UI files to process
set ( UI_FILES
  inc/MantidUiWidgetsCore/ManageUserDirectories.ui
  inc/MantidUiWidgetsCore/MWRunFiles.ui
  inc/MantidUiWidgetsCore/ScriptRepositoryView.ui

  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorOptionsDialog.ui
  inc/MantidUiWidgetsCore/DataProcessorUI/DataProcessorWidget.ui
  inc/MantidUiWidgetsCore/DataSelector.ui
  inc/MantidUiWidgetsCore/CatalogSearch.ui
  inc/MantidUiWidgetsCore/CatalogSelector.ui
  inc/MantidUiWidgetsCore/ColorBarWidget.ui
  inc/MantidUiWidgetsCore/DisplayCurveFit.ui
  inc/MantidUiWidgetsCore/IndirectInstrumentConfig.ui
  inc/MantidUiWidgetsCore/LogValueSelector.ui
  inc/MantidUiWidgetsCore/MWDiag.ui
  inc/MantidUiWidgetsCore/MWView.ui
  inc/MantidUiWidgetsCore/MultifitSetupDialog.ui
  inc/MantidUiWidgetsCore/MuonFitDataSelector.ui
  inc/MantidUiWidgetsCore/ProcessingAlgoWidget.ui
  inc/MantidUiWidgetsCore/RenameParDialog.ui
  inc/MantidUiWidgetsCore/SelectFunctionDialog.ui
  inc/MantidUiWidgetsCore/SequentialFitDialog.ui
  inc/MantidUiWidgetsCore/SlicingAlgorithmDialog.ui
  inc/MantidUiWidgetsCore/SlitCalculator.ui
  inc/MantidUiWidgetsCore/UserFunctionDialog.ui
  inc/MantidUiWidgetsCore/PeriodicTableWidget.ui
  inc/MantidUiWidgetsCore/PreviewPlot.ui
  inc/MantidUiWidgetsCore/pqHelpWindow.ui
  inc/MantidUiWidgetsCore/InstrumentView/UCorrectionDialog.ui
)

set ( TEST_FILES
   BatchAlgorithmRunnerTest.h
   FileDialogHandlerTest.h
   InterfaceManagerTest.h
   MantidColorMapTest.h
   NonOrthogonalTest.h
   PlotAxisTest.h
   QwtWorkspaceBinDataTest.h
   QwtWorkspaceSpectrumDataTest.h
   SelectionNotificationServiceTest.h
   SignalRangeTest.h
   SignalBlockerTest.h
  AlgorithmHintStrategyTest.h
  TrackedActionTest.h
  DataProcessorUI/DataProcessorCommandsTest.h
  DataProcessorUI/DataProcessorGenerateNotebookTest.h
  DataProcessorUI/DataProcessorOneLevelTreeManagerTest.h
  DataProcessorUI/DataProcessorPostprocessingAlgorithmTest.h
  DataProcessorUI/DataProcessorPreprocessingAlgorithmTest.h
  DataProcessorUI/DataProcessorProcessingAlgorithmBaseTest.h
  DataProcessorUI/DataProcessorProcessingAlgorithmTest.h
  DataProcessorUI/DataProcessorTwoLevelTreeManagerTest.h
  DataProcessorUI/DataProcessorWhiteListTest.h
  DataProcessorUI/GenericDataProcessorPresenterTest.h
  DataProcessorUI/ParseKeyValueStringTest.h
  DataProcessorUI/QDataProcessorOneLevelTreeModelTest.h
  DataProcessorUI/QDataProcessorTwoLevelTreeModelTest.h
  ProjectSaveModelTest.h
  ProjectSavePresenterTest.h
  WorkspacePresenter/ADSAdapterTest.h
  WorkspacePresenter/WorkspacePresenterTest.h
)

# Python unit tests
message ( WARNING " ============ NEED TO IMPLEMENT PYTHON MWRunFilesTest" )
#set ( TEST_PY_FILES
#  test/MWRunFilesTest.py
#)

include_directories ( inc )
include_directories ( ../qtpropertybrowser/src )
include_directories ( ../../MantidQt/Factory/inc )

qt4_wrap_cpp ( MOCCED_FILES ${MOC_FILES} )
set ( ALL_SRC ${SRC_FILES} ${MOCCED_FILES} )

# Generate headers from Ui files and include output directory in search path
qt4_wrap_ui ( UI_HDRS ${UI_FILES} )
include_directories ( ${CMAKE_CURRENT_BINARY_DIR} )

# resources
qt4_add_resources ( RES_FILES resources/core.qrc )

add_definitions ( -DQT_QTPROPERTYBROWSER_IMPORT )
# Use a precompiled header where they are supported
enable_precompiled_headers( inc/MantidUiWidgetsCore/PrecompiledHeader.h ALL_SRC )

set ( TARGET_NAME MantidUiWidgetsCore )
add_library ( ${TARGET_NAME} ${ALL_SRC} ${INC_FILES} ${UI_HDRS} ${RES_FILES} )

set_target_properties ( ${TARGET_NAME} PROPERTIES COMPILE_DEFINITIONS "IN_MANTIDUI_WIDGETS_CORE")

if (OSX_VERSION VERSION_GREATER 10.8)
  set_target_properties ( ${TARGET_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../MacOS")
endif ()

target_link_libraries ( ${TARGET_NAME} LINK_PRIVATE ${TCMALLOC_LIBRARIES_LINKTIME}
      QtPropertyBrowser
      ${CORE_MANTIDLIBS}
      ${QT_LIBRARIES} ${QWT_LIBRARIES}
      ${QSCINTILLA_LIBRARIES}
      ${POCO_LIBRARIES}
      ${Boost_LIBRARIES} ${PYTHON_LIBRARIES}
      ${ZLIB_LIBRARIES}
      ${OPENGL_LIBRARIES}
)

###########################################################################
# Testing
###########################################################################

add_subdirectory ( test )

###########################################################################
# Installation settings
###########################################################################

install ( TARGETS ${TARGET_NAME} ${SYSTEM_PACKAGE_TARGET} DESTINATION ${LIB_DIR} )
