# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2019 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
#  This file is part of the mantid workbench.

from __future__ import (absolute_import, unicode_literals)

from qtpy.QtWidgets import QWidget

from mantidqt.utils.qt import load_ui


class ImagesTabWidgetView(QWidget):

    def __init__(self, parent=None):
        super(ImagesTabWidgetView, self).__init__(parent=parent)

        self.ui = load_ui(__file__,
                          'images_tab.ui',
                          baseinstance=self)


class ImagesTabWidgetPresenter:

    def __init__(self, fig, parent=None):
        self.view = ImagesTabWidgetPresenter(parent)
        self.fig = fig
