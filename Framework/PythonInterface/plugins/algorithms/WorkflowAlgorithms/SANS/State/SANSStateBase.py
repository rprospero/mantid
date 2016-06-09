﻿from abc import (ABCMeta, abstractmethod)
import inspect
import copy
from mantid.kernel import PropertyManager


# ------------------------------------------------
# Free functions
# ------------------------------------------------
def get_descriptor_values(instance):
    # Get all descriptor names which are TypedParameter of instance's type
    descriptor_names = []
    for descriptor_name, descriptor_object in inspect.getmembers(type(instance)):
        if inspect.isdatadescriptor(descriptor_object) and isinstance(descriptor_object, TypedParameter):
            descriptor_names.append(descriptor_name)

    # Get the descriptor values from the instance
    descriptor_values = {}
    for key in descriptor_names:
        if hasattr(instance, key):
            value = getattr(instance, key)
            descriptor_values.update({key: value})
    return descriptor_values


def convert_state_to_property_manager(instance):
    descriptor_values = get_descriptor_values(instance)

    # Add the descriptors to a PropertyManager object
    property_manager = PropertyManager()
    for key in descriptor_values:
        value = descriptor_values[key]
        if value is not None:
            property_manager.declareProperty(key, value)
    return property_manager


def convert_state_to_dict(instance):
    descriptor_values = get_descriptor_values(instance)
    # Add the descriptors to a dict
    state_dict = dict()
    for key, value in descriptor_values.iteritems():
        # If the value is a SANSBaseState then create a dict from it
        if isinstance(value, SANSStateBase):
            sub_state_dict = convert_state_to_dict(value)
            value = sub_state_dict
        state_dict.update({key: value})
    return state_dict


def set_state_from_property_manager(instance, property_manager):
    keys = property_manager.keys()
    for key in keys:
        value = property_manager.getProperty(key).value
        setattr(instance, key, value)


# -------------------------------------------------------
# Parameters
# -------------------------------------------------------
class TypedParameter(object):
    def __init__(self, name, parameter_type, validator=lambda x: True):
        self._typed_parameter_name = name
        self.name = "_" + name
        self.parameter_type = parameter_type
        self.value = None
        self.validator = validator

    def __get__(self, instance, owner):
        if instance is None:
            return self
        else:
            if hasattr(instance, self.name):
                return getattr(instance, self.name)
            else:
                return None

    def __set__(self, instance, value):
        if not isinstance(value, self.parameter_type):
            raise TypeError("Trying to set {} which expects a value of type {}."
                            " Got a value of {} which is of type: {}".format(self.name, str(self.parameter_type),
                                                                             str(value), str(type(value))))

        if self.validator(value):
            # The descriptor should be holding onto its own copy
            copied_value = copy.deepcopy(value)
            setattr(instance, self.name, copied_value)
        else:
            raise ValueError("Trying to set {} with an invalid value of {}".format(self.name, str(value)))

    def __delete__(self):
        raise AttributeError("Cannot delete the attribute {}".format(self.name))


# ---------------------------------------------------------------
# Validator functions
# ---------------------------------------------------------------
def is_not_none(value):
    return value is not None


def is_positive(value):
    return value >= 0


# ------------------------------------------------
# SANSStateBase
# ------------------------------------------------
class SANSStateBase(object):
    __metaclass__ = ABCMeta

    @property
    @abstractmethod
    def property_manager(self):
        pass

    @property_manager.setter
    @abstractmethod
    def property_manager(self, value):
        pass

    @abstractmethod
    def validate(self):
        pass
