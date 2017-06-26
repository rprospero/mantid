from sans.user_file.user_file_common import (OtherId, DetectorId, LimitsId, SetId, SampleId,
                                             event_binning_string_values, set_scales_entry)
from sans.common.enums import (ReductionDimensionality, ISISReductionMode, RangeStepType, SampleShape, SaveType)
from sans.user_file.user_file_common import (simple_range)


class StateGuiModel(object):
    def __init__(self, user_file_items):
        super(StateGuiModel, self).__init__()
        self._user_file_items = user_file_items

    @property
    def settings(self):
        return self._user_file_items

    def get_simple_element(self, element_id, default_value):
        return self.get_simple_element_with_attribute(element_id, default_value)

    def set_simple_element(self, element_id, value):
        if element_id in self._user_file_items:
            del self._user_file_items[element_id]
        new_state_entries = {element_id: [value]}
        self._user_file_items.update(new_state_entries)

    def get_simple_element_with_attribute(self, element_id, default_value, attribute=None):
        if element_id in self._user_file_items:
            element = self._user_file_items[element_id][-1]
            return getattr(element, attribute) if attribute else element
        else:
            return default_value

    # ------------------------------------------------------------------------------------------------------------------
    # Event slices
    # ------------------------------------------------------------------------------------------------------------------
    @property
    def event_slices(self):
        return self.get_simple_element_with_attribute(element_id=OtherId.event_slices,
                                                      default_value="",
                                                      attribute="value")

    @event_slices.setter
    def event_slices(self, value):
        if not value:
            return
        if OtherId.event_slices in self._user_file_items:
            del self._user_file_items[OtherId.event_slices]
        new_state_entries = {OtherId.event_slices: [event_binning_string_values(value=value)]}
        self._user_file_items.update(new_state_entries)

    # ------------------------------------------------------------------------------------------------------------------
    # Reduction dimensionality
    # ------------------------------------------------------------------------------------------------------------------
    @property
    def reduction_dimensionality(self):
        return self.get_simple_element_with_attribute(element_id=OtherId.reduction_dimensionality,
                                                      default_value=ReductionDimensionality.OneDim)

    @reduction_dimensionality.setter
    def reduction_dimensionality(self, value):
        if value is ReductionDimensionality.OneDim or value is ReductionDimensionality.TwoDim:
            if OtherId.reduction_dimensionality in self._user_file_items:
                del self._user_file_items[OtherId.reduction_dimensionality]
            new_state_entries = {OtherId.reduction_dimensionality: [value]}
            self._user_file_items.update(new_state_entries)
        else:
            raise ValueError("A reduction dimensionality was expected, got instead {}".format(value))

    # ------------------------------------------------------------------------------------------------------------------
    # Reduction Mode
    # ------------------------------------------------------------------------------------------------------------------
    @property
    def reduction_mode(self):
        return self.get_simple_element_with_attribute(element_id=DetectorId.reduction_mode,
                                                      default_value=ISISReductionMode.LAB)

    @reduction_mode.setter
    def reduction_mode(self, value):
        if (value is ISISReductionMode.LAB or value is ISISReductionMode.HAB or
            value is ISISReductionMode.Merged or value is ISISReductionMode.All):  # noqa
            if DetectorId.reduction_mode in self._user_file_items:
                del self._user_file_items[DetectorId.reduction_mode]
            new_state_entries = {DetectorId.reduction_mode: [value]}
            self._user_file_items.update(new_state_entries)
        else:
            raise ValueError("A reduction mode was expected, got instead {}".format(value))

    # ------------------------------------------------------------------------------------------------------------------
    # Wavelength properties
    # ------------------------------------------------------------------------------------------------------------------
    def _update_wavelength(self, min_value=None, max_value=None, step=None, step_type=None):
        if LimitsId.wavelength in self._user_file_items:
            settings = self._user_file_items[LimitsId.wavelength]
        else:
            # If the entry does not already exist, then add it. The -1. is an illegal input which should get overriden
            # and if not we want it to fail.
            settings = [simple_range(start=-1., stop=-1., step=-1., step_type=RangeStepType.Lin)]

        new_settings = []
        for setting in settings:
            new_min = min_value if min_value else setting.start
            new_max = max_value if max_value else setting.stop
            new_step = step if step else setting.step
            new_step_type = step_type if step_type else setting.step_type
            new_setting = simple_range(start=new_min, stop=new_max, step=new_step, step_type=new_step_type)
            new_settings.append(new_setting)
        self._user_file_items.update({LimitsId.wavelength: new_settings})

    @property
    def wavelength_step_type(self):
        return self.get_simple_element_with_attribute(element_id=LimitsId.wavelength, default_value=RangeStepType.Lin,
                                                      attribute="step_type")

    @wavelength_step_type.setter
    def wavelength_step_type(self, value):
        self._update_wavelength(step_type=value)

    @property
    def wavelength_min(self):
        return self.get_simple_element_with_attribute(element_id=LimitsId.wavelength,
                                                      default_value="",
                                                      attribute="start")

    @wavelength_min.setter
    def wavelength_min(self, value):
        self._update_wavelength(min_value=value)

    @property
    def wavelength_max(self):
        return self.get_simple_element_with_attribute(element_id=LimitsId.wavelength,
                                                      default_value="",
                                                      attribute="stop")

    @wavelength_max.setter
    def wavelength_max(self, value):
        self._update_wavelength(max_value=value)

    @property
    def wavelength_step(self):
        return self.get_simple_element_with_attribute(element_id=LimitsId.wavelength,
                                                      default_value="",
                                                      attribute="step")

    @wavelength_step.setter
    def wavelength_step(self, value):
        self._update_wavelength(step=value)

    # ------------------------------------------------------------------------------------------------------------------
    # Scale properties
    # While the absolute scale can be set in the
    # ------------------------------------------------------------------------------------------------------------------
    @property
    def absolute_scale(self):
        return self.get_simple_element_with_attribute(element_id=SetId.scales,
                                                      default_value="",
                                                      attribute="s")

    @absolute_scale.setter
    def absolute_scale(self, value):
        if SetId.scales in self._user_file_items:
            settings = self._user_file_items[SetId.scales]
        else:
            settings = [set_scales_entry(s=100., a=0., b=0., c=0., d=0.)]

        new_settings = []
        for setting in settings:
            s_parameter = value if value else setting.s
            new_settings.append(set_scales_entry(s=s_parameter, a=0., b=0., c=0., d=0.))
        self._user_file_items.update({SetId.scales: new_settings})

    @property
    def sample_height(self):
        return self.get_simple_element(element_id=OtherId.sample_height, default_value="")

    @sample_height.setter
    def sample_height(self, value):
        self.set_simple_element(element_id=OtherId.sample_height, value=value)

    @property
    def sample_width(self):
        return self.get_simple_element(element_id=OtherId.sample_width, default_value="")

    @sample_width.setter
    def sample_width(self, value):
        self.set_simple_element(element_id=OtherId.sample_width, value=value)

    @property
    def sample_thickness(self):
        return self.get_simple_element(element_id=OtherId.sample_thickness, default_value="")

    @sample_thickness.setter
    def sample_thickness(self, value):
        self.set_simple_element(element_id=OtherId.sample_thickness, value=value)

    @property
    def sample_shape(self):
        return self.get_simple_element(element_id=OtherId.sample_shape, default_value=SampleShape.CylinderAxisUp)

    @sample_shape.setter
    def sample_shape(self, value):
        # We only set the value if it is not None. Note that it can be None if the sample shape selection
        #  is "Read from file"
        if value is not None:
            self.set_simple_element(element_id=OtherId.sample_shape, value=value)

    @property
    def z_offset(self):
        return self.get_simple_element(element_id=SampleId.offset, default_value="")

    @z_offset.setter
    def z_offset(self, value):
        self.set_simple_element(element_id=SampleId.offset, value=value)

    # ------------------------------------------------------------------------------------------------------------------
    # Compatibility Mode Options
    # ------------------------------------------------------------------------------------------------------------------
    @property
    def compatibility_mode(self):
        return self.get_simple_element(element_id=OtherId.use_compatibility_mode, default_value=False)

    @compatibility_mode.setter
    def compatibility_mode(self, value):
        self.set_simple_element(element_id=OtherId.use_compatibility_mode, value=value)

    # ------------------------------------------------------------------------------------------------------------------
    # Save Options
    # ------------------------------------------------------------------------------------------------------------------
    @property
    def zero_error_free(self):
        if OtherId.save_as_zero_error_free in self._user_file_items:
            return self._user_file_items[OtherId.save_as_zero_error_free][-1]
        else:
            # Turn on zero error free saving by default
            return True

    @zero_error_free.setter
    def zero_error_free(self, value):
        if value is None:
            return
        if OtherId.save_as_zero_error_free in self._user_file_items:
            del self._user_file_items[OtherId.save_as_zero_error_free]
        new_state_entries = {OtherId.save_as_zero_error_free: [value]}
        self._user_file_items.update(new_state_entries)

    @property
    def save_types(self):
        return self.get_simple_element(element_id=OtherId.save_types, default_value=[SaveType.NXcanSAS])

    @save_types.setter
    def save_types(self, value):
        self.set_simple_element(element_id=OtherId.save_types, value=value)
