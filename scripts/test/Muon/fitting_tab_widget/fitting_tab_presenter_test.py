import unittest
from mantid.py3compat import mock
from mantidqt.utils.qt.testing import GuiTest
from qtpy import QtWidgets
from Muon.GUI.Common.fitting_tab_widget.fitting_tab_widget import FittingTabWidget
from Muon.GUI.Common.test_helpers.context_setup import setup_context
from mantid.api import FunctionFactory


def retrieve_combobox_info(combo_box):
    output_list = []
    for i in range(combo_box.count()):
        output_list.append(str(combo_box.itemText(i)))

    return output_list


def wait_for_thread(thread_model):
    if thread_model:
        thread_model._thread.wait()
        QtWidgets.QApplication.instance().processEvents()


class FittingTabPresenterTest(GuiTest):
    def setUp(self):
        self.context = setup_context()
        self.context.data_context.current_runs = [[62260]]
        self.context.data_context.instrument = 'MUSR'
        self.widget = FittingTabWidget(self.context, parent=None)
        self.presenter = self.widget.fitting_tab_presenter
        self.view = self.widget.fitting_tab_view
        self.presenter.model = mock.MagicMock()

    @mock.patch('Muon.GUI.Common.fitting_tab_widget.fitting_tab_presenter.WorkspaceSelectorView.get_selected_data')
    def test_handle_select_fit_data_clicked_updates_current_run_list(self, dialog_mock):
        dialog_mock.return_value = (['MUSR62260; Group; bkwd; Asymmetry; #1', 'MUSR62260; Group; bottom; Asymmetry; #1',
                                     'MUSR62260; Group; fwd; Asymmetry; #1', 'MUSR62260; Group; top; Asymmetry; #1',
                                     'MUSR62260; Pair Asym; long; #1', 'MUSR62260; PhaseQuad; PhaseTable MUSR62260',
                                     'MUSR62260; PhaseQuad; PhaseTable MUSR62261'], True)

        self.presenter.handle_select_fit_data_clicked()

        dialog_mock.assert_called_once_with([[62260]], 'MUSR', [], True, self.context, self.view)

        self.assertEqual(retrieve_combobox_info(self.view.parameter_display_combo),
                         ['MUSR62260; Group; bkwd; Asymmetry; #1', 'MUSR62260; Group; bottom; Asymmetry; #1',
                          'MUSR62260; Group; fwd; Asymmetry; #1', 'MUSR62260; Group; top; Asymmetry; #1',
                          'MUSR62260; Pair Asym; long; #1', 'MUSR62260; PhaseQuad; PhaseTable MUSR62260',
                          'MUSR62260; PhaseQuad; PhaseTable MUSR62261'])

    def test_that_changeing_fitting_type_to_multiple_fit_changes_workspace_selector_combo_label(
            self):
        self.assertEqual(self.view.workspace_combo_box_label.text(), 'Select Workspace')
        self.view.sequential_fit_radio.toggle()

        self.assertEqual(self.view.workspace_combo_box_label.text(), 'Display parameters for')

    def test_that_changeing_fit_type_to_single_fit_updates_label(self):
        self.view.sequential_fit_radio.toggle()
        self.view.single_fit_radio.toggle()

        self.assertEqual(self.view.workspace_combo_box_label.text(), 'Select Workspace')

    def test_when_fit_is_clicked_in_single_mode_the_fit_function_string_workspace_and_additional_options_are_passed_to_fit(
            self):
        self.presenter.selected_data = ['Input Workspace Name']
        self.view.function_browser.setFunction('name=GausOsc,A=0.2,Sigma=0.2,Frequency=0.1,Phi=0')

        self.view.fit_button.clicked.emit(True)
        wait_for_thread(self.presenter.calculation_thread)

        self.presenter.model.do_single_fit.assert_called_once_with(
            {'Function': mock.ANY, 'InputWorkspace': 'Input Workspace Name',
             'Minimizer': 'Levenberg-Marquardt', 'StartX': 0.0, 'EndX': 15.0, 'EvaluationType': 'CentrePoint'})

        self.assertEqual(str(self.presenter.model.do_single_fit.call_args[0][0]['Function']), 'name=GausOsc,A=0.2,Sigma=0.2,Frequency=0.1,Phi=0')

    def test_get_parameters_for_single_fit_returns_correctly(self):
        self.presenter.selected_data = ['Input Workspace Name']
        self.view.function_browser.setFunction('name=GausOsc,A=0.2,Sigma=0.2,Frequency=0.1,Phi=0')
        result = self.presenter.get_parameters_for_single_fit()

        self.assertEqual(result, {'Function': mock.ANY,
                                  'InputWorkspace': 'Input Workspace Name',
                                  'Minimizer': 'Levenberg-Marquardt', 'StartX': 0.0, 'EndX': 15.0,
                                  'EvaluationType': 'CentrePoint'}
                         )

    def test_for_single_fit_mode_when_display_workspace_changes_updates_fitting_browser_with_new_name(self):
        self.presenter.selected_data = ['Input Workspace Name']

        self.presenter.handle_display_workspace_changed()

        self.assertEqual(self.view.function_browser.getDatasetNames(), ['Input Workspace Name'])

    def test_fit_clicked_with_simultaneous_selected(self):
        self.view.simul_fit_radio.toggle()
        self.presenter.selected_data = ['Input Workspace Name_1', 'Input Workspace Name 2']
        self.view.function_browser.setFunction('name=GausOsc,A=0.2,Sigma=0.2,Frequency=0.1,Phi=0')
        trial_function = FunctionFactory.createInitialized(
            'composite=MultiDomainFunction,NumDeriv=true;name=GausOsc,A=0.2,Sigma=0.2,Frequency=0.1,Phi=0,'
            '$domains=i;name=GausOsc,A=0.2,Sigma=0.2,Frequency=0.1,Phi=0,$domains=i')

        self.view.fit_button.clicked.emit(True)
        wait_for_thread(self.presenter.calculation_thread)
        call_args_dict = self.presenter.model.do_simultaneous_fit.call_args[0][0]

        self.assertEqual(call_args_dict['InputWorkspace'], ['Input Workspace Name_1', 'Input Workspace Name 2'])
        self.assertEqual(call_args_dict['Minimizer'], 'Levenberg-Marquardt')
        self.assertEqual(call_args_dict['StartX'], [0.0, 0.0])
        self.assertEqual(call_args_dict['EndX'], [15.0, 15.0])

    def test_when_new_data_is_selected_clear_out_old_fits_and_information(self):
        self.presenter._fit_status = ['success', 'success', 'success']
        self.presenter._fit_chi_squared = [12.3, 3.4, 0.35]
        fit_function = FunctionFactory.createInitialized('name=GausOsc,A=0.2,Sigma=0.2,Frequency=0.1,Phi=0')
        self.presenter_fit_function = [fit_function, fit_function, fit_function]
        self.presenter.manual_selection_made = True
        self.presenter._start_x = [0.15, 0.45, 0.67]
        self.presenter._end_x = [0.56, 0.78, 0.34]
        self.view.end_time = 0.56
        self.view.start_time = 0.15
        new_workspace_list = ['MUSR22725; Group; top; Asymmetry', 'MUSR22725; Group; bottom; Asymmetry',
                              'MUSR22725; Group; fwd; Asymmetry']

        self.presenter.handle_workspace_list_changed(new_workspace_list)

        self.assertEqual(self.presenter._fit_status, ['no fit', 'no fit', 'no fit'])
        self.assertEqual(self.presenter._fit_chi_squared, [0.0, 0.0, 0.0])
        self.assertEqual(self.presenter._fit_function, [None, None, None])
        self.assertEqual(self.presenter._selected_data, new_workspace_list)
        self.assertEqual(self.presenter.manual_selection_made, False)
        self.assertEqual(self.presenter.start_x, [0.15, 0.15, 0.15])
        self.assertEqual(self.presenter.end_x, [0.56, 0.56, 0.56])

    def test_when_new_data_is_selected_updates_combo_box_on_view(self):
        new_workspace_list = ['MUSR22725; Group; top; Asymmetry', 'MUSR22725; Group; bottom; Asymmetry',
                              'MUSR22725; Group; fwd; Asymmetry']

        self.presenter.handle_workspace_list_changed(new_workspace_list)

        self.assertEqual(retrieve_combobox_info(self.view.parameter_display_combo), new_workspace_list)

    # def test_when_new_data_is_selected


if __name__ == '__main__':
    unittest.main(buffer=False, verbosity=2)
