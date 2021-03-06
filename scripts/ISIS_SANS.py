# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
#   NScD Oak Ridge National Laboratory, European Spallation Source,
#   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
# SPDX - License - Identifier: GPL - 3.0 +
#pylint: disable=invalid-name
"""
    Script used to start the Test Interface from MantidPlot
"""
from sans.common.enums import SANSFacility
from sans.gui_logic.presenter.run_tab_presenter import RunTabPresenter
from ui.sans_isis import sans_data_processor_gui


main_window_view = sans_data_processor_gui.SANSDataProcessorGui()

run_tab_presenter = RunTabPresenter(SANSFacility.ISIS, view=main_window_view)


# Show
main_window_view.show()
