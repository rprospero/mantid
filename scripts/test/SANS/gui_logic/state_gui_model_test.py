from __future__ import (absolute_import, division, print_function)
import unittest
import mantid
from sans.gui_logic.models.state_gui_model import StateGuiModel
from sans.user_file.user_file_common import (OtherId, event_binning_string_values, DetectorId)
from sans.common.enums import (ReductionDimensionality, ISISReductionMode, RangeStepType, SampleShape, SaveType)


class StateGuiModelTest(unittest.TestCase):
    # ------------------------------------------------------------------------------------------------------------------
    # Event slices
    # ------------------------------------------------------------------------------------------------------------------
    def test_that_if_no_slice_event_is_present_an_empty_string_is_returned(self):
        state_gui_model = StateGuiModel({"test": 1})
        self.assertTrue(state_gui_model.event_slices == "")

    def test_that_slice_event_can_be_retrieved_if_it_exists(self):
        state_gui_model = StateGuiModel({OtherId.event_slices: [event_binning_string_values(value="test")]})
        self.assertTrue(state_gui_model.event_slices == "test")

    def test_that_slice_event_can_be_updated(self):
        state_gui_model = StateGuiModel({OtherId.event_slices: [event_binning_string_values(value="test")]})
        state_gui_model.event_slices = "test2"
        self.assertTrue(state_gui_model.event_slices == "test2")

    # ------------------------------------------------------------------------------------------------------------------
    # Reduction dimensionality
    # ------------------------------------------------------------------------------------------------------------------
    def test_that_is_1D_reduction_by_default(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertTrue(state_gui_model.reduction_dimensionality is ReductionDimensionality.OneDim)

    def test_that_is_set_to_2D_reduction(self):
        state_gui_model = StateGuiModel({"test": [1]})
        state_gui_model.reduction_dimensionality = ReductionDimensionality.TwoDim
        self.assertTrue(state_gui_model.reduction_dimensionality is ReductionDimensionality.TwoDim)

    def test_that_raises_when_not_setting_with_reduction_dim_enum(self):
        def red_dim_wrapper():
            state_gui_model = StateGuiModel({"test": [1]})
            state_gui_model.reduction_dimensionality = "string"
        self.assertRaises(ValueError, red_dim_wrapper)

    def test_that_can_update_reduction_dimensionality(self):
        state_gui_model = StateGuiModel({OtherId.reduction_dimensionality: [ReductionDimensionality.OneDim]})
        self.assertTrue(state_gui_model.reduction_dimensionality is ReductionDimensionality.OneDim)
        state_gui_model.reduction_dimensionality = ReductionDimensionality.TwoDim
        self.assertTrue(state_gui_model.reduction_dimensionality is ReductionDimensionality.TwoDim)

    # ------------------------------------------------------------------------------------------------------------------
    # Reduction mode
    # ------------------------------------------------------------------------------------------------------------------
    def test_that_is_set_to_lab_by_default(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertTrue(state_gui_model.reduction_mode is ISISReductionMode.LAB)

    def test_that_can_be_set_to_something_else(self):
        state_gui_model = StateGuiModel({"test": [1]})
        state_gui_model.reduction_mode = ISISReductionMode.Merged
        self.assertTrue(state_gui_model.reduction_mode is ISISReductionMode.Merged)

    def test_that_raises_when_setting_with_wrong_input(self):
        def red_mode_wrapper():
            state_gui_model = StateGuiModel({"test": [1]})
            state_gui_model.reduction_mode = "string"
        self.assertRaises(ValueError, red_mode_wrapper)

    def test_that_can_update_reduction_mode(self):
        state_gui_model = StateGuiModel({DetectorId.reduction_mode: [ISISReductionMode.HAB]})
        self.assertTrue(state_gui_model.reduction_mode is ISISReductionMode.HAB)
        state_gui_model.reduction_mode = ISISReductionMode.All
        self.assertTrue(state_gui_model.reduction_mode is ISISReductionMode.All)

    # ------------------------------------------------------------------------------------------------------------------
    # Wavelength
    # ------------------------------------------------------------------------------------------------------------------
    def test_that_default_wavelength_settings_are_empty(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertTrue(not state_gui_model.wavelength_min)
        self.assertTrue(not state_gui_model.wavelength_max)
        self.assertTrue(not state_gui_model.wavelength_step)

    def test_that_default_wavelength_step_type_is_linear(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertTrue(state_gui_model.wavelength_step_type is RangeStepType.Lin)

    def test_that_can_set_wavelength(self):
        state_gui_model = StateGuiModel({"test": [1]})
        state_gui_model.wavelength_min = 1.
        state_gui_model.wavelength_max = 2.
        state_gui_model.wavelength_step = .5
        state_gui_model.wavelength_step_type = RangeStepType.Lin
        state_gui_model.wavelength_step_type = RangeStepType.Log
        self.assertTrue(state_gui_model.wavelength_min == 1.)
        self.assertTrue(state_gui_model.wavelength_max == 2.)
        self.assertTrue(state_gui_model.wavelength_step == .5)
        self.assertTrue(state_gui_model.wavelength_step_type is RangeStepType.Log)

    # ------------------------------------------------------------------------------------------------------------------
    # Scale
    # ------------------------------------------------------------------------------------------------------------------
    def test_that_absolute_scale_has_an_empty_default_value(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertTrue(not state_gui_model.absolute_scale)

    def test_that_can_set_absolute_scale(self):
        state_gui_model = StateGuiModel({"test": [1]})
        state_gui_model.absolute_scale = .5
        self.assertTrue(state_gui_model.absolute_scale == .5)

    def test_that_default_extents_are_empty(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertTrue(not state_gui_model.sample_width)
        self.assertTrue(not state_gui_model.sample_height)
        self.assertTrue(not state_gui_model.sample_thickness)
        self.assertTrue(not state_gui_model.z_offset)

    def test_that_default_sample_shape_is_cylinder_axis_up(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertTrue(state_gui_model.sample_shape is SampleShape.CylinderAxisUp)

    def test_that_can_set_the_sample_geometry(self):
        state_gui_model = StateGuiModel({"test": [1]})
        state_gui_model.sample_width = 1.2
        state_gui_model.sample_height = 1.6
        state_gui_model.sample_thickness = 1.8
        state_gui_model.z_offset = 1.78
        state_gui_model.sample_shape = SampleShape.Cuboid
        self.assertTrue(state_gui_model.sample_width == 1.2)
        self.assertTrue(state_gui_model.sample_height == 1.6)
        self.assertTrue(state_gui_model.sample_thickness == 1.8)
        self.assertTrue(state_gui_model.z_offset == 1.78)
        self.assertTrue(state_gui_model.sample_shape is SampleShape.Cuboid)

    # ------------------------------------------------------------------------------------------------------------------
    # Compatibility Mode
    # ------------------------------------------------------------------------------------------------------------------
    def test_that_default_compatibility_mode_is_false(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertFalse(state_gui_model.compatibility_mode)

    def test_that_can_set_compatibility_mode(self):
        state_gui_model = StateGuiModel({"test": [1]})
        state_gui_model.compatibility_mode = True
        self.assertTrue(state_gui_model.compatibility_mode)

    # ------------------------------------------------------------------------------------------------------------------
    # Save options
    # ------------------------------------------------------------------------------------------------------------------
    def test_that_can_zero_error_free_saving_is_default(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertTrue(state_gui_model.zero_error_free)

    def test_that_can_zero_error_free_saving_can_be_changed(self):
        state_gui_model = StateGuiModel({OtherId.save_as_zero_error_free: [True]})
        state_gui_model.zero_error_free = False
        self.assertFalse(state_gui_model.zero_error_free)

    def test_that_default_save_type_is_NXcanSAS(self):
        state_gui_model = StateGuiModel({"test": [1]})
        self.assertTrue(state_gui_model.save_types == [SaveType.NXcanSAS])

    def test_that_can_select_multiple_save_types(self):
        state_gui_model = StateGuiModel({"test": [1]})
        state_gui_model.save_types = [SaveType.RKH, SaveType.NXcanSAS]
        self.assertTrue(state_gui_model.save_types == [SaveType.RKH, SaveType.NXcanSAS])


if __name__ == '__main__':
    unittest.main()


