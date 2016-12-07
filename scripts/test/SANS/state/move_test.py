import unittest
import mantid

from sans.state.move import (StateMoveLOQ, StateMoveSANS2D, StateMoveLARMOR, StateMove, get_move_builder)
from sans.state.data import get_data_builder
from sans.common.constants import SANSConstants
from sans.common.sans_type import (CanonicalCoordinates, SANSFacility)
from state_test_helper import assert_validate_error, assert_raises_nothing


# ----------------------------------------------------------------------------------------------------------------------
# State
# ----------------------------------------------------------------------------------------------------------------------
class StateMoveWorkspaceTest(unittest.TestCase):
    def test_that_raises_if_the_detector_name_is_not_set_up(self):
        state = StateMove()
        state.detectors[SANSConstants.low_angle_bank].detector_name = "test"
        state.detectors[SANSConstants.high_angle_bank].detector_name_short = "test"
        state.detectors[SANSConstants.low_angle_bank].detector_name_short = "test"
        assert_validate_error(self, ValueError, state)
        state.detectors[SANSConstants.high_angle_bank].detector_name = "test"
        assert_raises_nothing(self, state)

    def test_that_raises_if_the_short_detector_name_is_not_set_up(self):
        state = StateMove()
        state.detectors[SANSConstants.high_angle_bank].detector_name = "test"
        state.detectors[SANSConstants.low_angle_bank].detector_name = "test"
        state.detectors[SANSConstants.high_angle_bank].detector_name_short = "test"
        assert_validate_error(self, ValueError, state)
        state.detectors[SANSConstants.low_angle_bank].detector_name_short = "test"
        assert_raises_nothing(self, state)

    def test_that_general_isis_default_values_are_set_up(self):
        state = StateMove()
        self.assertTrue(state.sample_offset == 0.0)
        self.assertTrue(state.sample_offset_direction is CanonicalCoordinates.Z)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].x_translation_correction == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].y_translation_correction == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].z_translation_correction == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].rotation_correction == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].side_correction == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].radius_correction == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].x_tilt_correction == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].y_tilt_correction == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].z_tilt_correction == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].sample_centre_pos1 == 0.0)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].sample_centre_pos2 == 0.0)


class StateMoveWorkspaceLOQTest(unittest.TestCase):
    def test_that_is_sans_state_move_object(self):
        state = StateMoveLOQ()
        self.assertTrue(isinstance(state, StateMove))

    def test_that_LOQ_has_centre_position_set_up(self):
        state = StateMoveLOQ()
        self.assertTrue(state.center_position == 317.5 / 1000.)
        self.assertTrue(state.monitor_names == {})


class StateMoveWorkspaceSANS2DTest(unittest.TestCase):
    def test_that_is_sans_state_data_object(self):
        state = StateMoveSANS2D()
        self.assertTrue(isinstance(state, StateMove))

    def test_that_sans2d_has_default_values_set_up(self):
        # Arrange
        state = StateMoveSANS2D()
        self.assertTrue(state.hab_detector_radius == 306.0/1000.)
        self.assertTrue(state.hab_detector_default_sd_m == 4.0)
        self.assertTrue(state.hab_detector_default_x_m == 1.1)
        self.assertTrue(state.lab_detector_default_sd_m == 4.0)
        self.assertTrue(state.hab_detector_x == 0.0)
        self.assertTrue(state.hab_detector_z == 0.0)
        self.assertTrue(state.hab_detector_rotation == 0.0)
        self.assertTrue(state.lab_detector_x == 0.0)
        self.assertTrue(state.lab_detector_z == 0.0)
        self.assertTrue(state.monitor_names == {})
        self.assertTrue(state.monitor_4_offset == 0.0)


class StateMoveWorkspaceLARMORTest(unittest.TestCase):
    def test_that_is_sans_state_data_object(self):
        state = StateMoveLARMOR()
        self.assertTrue(isinstance(state, StateMove))

    def test_that_can_set_and_get_values(self):
        state = StateMoveLARMOR()
        self.assertTrue(state.bench_rotation == 0.0)
        self.assertTrue(state.monitor_names == {})


# ----------------------------------------------------------------------------------------------------------------------
# Builder
# ----------------------------------------------------------------------------------------------------------------------
class StateMoveBuilderTest(unittest.TestCase):
    def test_that_state_for_loq_can_be_built(self):
        # Arrange
        facility = SANSFacility.ISIS
        data_builder = get_data_builder(facility)
        data_builder.set_sample_scatter("LOQ74044")
        data_builder.set_sample_scatter_period(3)
        data_info = data_builder.build()

        # Act
        builder = get_move_builder(data_info)
        self.assertTrue(builder)
        value = 324.2
        builder.set_center_position(value)
        builder.set_HAB_x_translation_correction(value)
        builder.set_LAB_sample_centre_pos1(value)

        # Assert
        state = builder.build()
        self.assertTrue(state.center_position == value)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].x_translation_correction == value)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].detector_name_short == "HAB")
        self.assertTrue(state.detectors[SANSConstants.low_angle_bank].detector_name == "main-detector-bank")
        self.assertTrue(state.monitor_names[str(2)] == "monitor2")
        self.assertTrue(len(state.monitor_names) == 2)
        self.assertTrue(state.detectors[SANSConstants.low_angle_bank].sample_centre_pos1 == value)

    def test_that_state_for_sans2d_can_be_built(self):
        # Arrange
        facility = SANSFacility.ISIS
        data_builder = get_data_builder(facility)
        data_builder.set_sample_scatter("SANS2D00022048")
        data_info = data_builder.build()

        # Act
        builder = get_move_builder(data_info)
        self.assertTrue(builder)
        value = 324.2
        builder.set_HAB_x_translation_correction(value)

        # Assert
        state = builder.build()
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].x_translation_correction == value)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].detector_name_short == "front")
        self.assertTrue(state.detectors[SANSConstants.low_angle_bank].detector_name == "rear-detector")
        self.assertTrue(state.monitor_names[str(7)] == "monitor7")
        self.assertTrue(len(state.monitor_names) == 8)

    def test_that_state_for_larmor_can_be_built(self):
        # Arrange
        facility = SANSFacility.ISIS
        data_builder = get_data_builder(facility)
        data_builder.set_sample_scatter("LARMOR00002260")
        data_info = data_builder.build()

        # Act
        builder = get_move_builder(data_info)
        self.assertTrue(builder)
        value = 324.2
        builder.set_HAB_x_translation_correction(value)

        # Assert
        state = builder.build()
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].x_translation_correction == value)
        self.assertTrue(state.detectors[SANSConstants.high_angle_bank].detector_name_short == "front")
        self.assertTrue(state.detectors[SANSConstants.low_angle_bank].detector_name == "DetectorBench")
        self.assertTrue(state.monitor_names[str(5)] == "monitor5")
        self.assertTrue(len(state.monitor_names) == 10)


if __name__ == '__main__':
    unittest.main()
