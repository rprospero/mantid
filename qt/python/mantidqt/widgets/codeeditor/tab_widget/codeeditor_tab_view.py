# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2019 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
#  This file is part of the mantidqt package
from __future__ import absolute_import

from qtpy.QtCore import QPoint, Qt
from qtpy.QtWidgets import QAction, QMenu, QTabBar, QTabWidget, QToolButton

from mantidqt.icons import get_icon
from mantidqt.utils.qt import add_actions, create_action
from mantidqt.widgets.codeeditor.tab_widget.codeeditor_tab_presenter import CodeEditorTabPresenter


class CodeEditorTabWidget(QTabWidget):
    def __init__(self, parent=None, presenter=None):
        self.presenter = presenter if presenter else CodeEditorTabPresenter(self)
        super(CodeEditorTabWidget, self).__init__(parent)

        self.setAttribute(Qt.WA_DeleteOnClose, True)
        self.setContextMenuPolicy(Qt.ActionsContextMenu)
        self.setMovable(True)
        self.setTabsClosable(True)
        self.setDocumentMode(True)

        self.last_tab_clicked = 0

        # self.setStyleSheet("""
        #  QTabBar::tab{
        #  padding: 3px 0 3px 0;
        #  }
        # """)
        # QTabWidget::pane { /* The tab widget frame */
        #     border-top: 2px solid #C2C7CB;
        #     position: absolute;
        #     top: 0.5em;
        # }
        # QTabWidget::tab-bar {
        #     left: 5px; /* move to the right by 5px */
        # }
        # """)

        self.setup_tabs_context_menu(parent)
        self.setup_options_actions(parent)

        # find the QTabBar inside the QTabWidget to disable drawing the base,
        # which prevents stacking multiple borders between the tabs and the code widget
        tab_bar = self.findChild(QTabBar, "qt_tabwidget_tabbar")
        tab_bar.setDrawBase(False)

        # create a button to add new tabs
        plus_btn = QToolButton(self)
        plus_btn.clicked.connect(parent.plus_button_clicked)
        plus_btn.setIcon(get_icon("fa.plus"))
        self.setCornerWidget(plus_btn, Qt.TopLeftCorner)

    def setup_tabs_context_menu(self, parent):
        """
        Setup the actions for the context menu (right click). These are handled by the presenter
        """
        self.tabBarClicked.connect(self.tab_was_clicked)

        show_in_explorer = QAction("Show in Explorer", self)
        show_in_explorer.triggered.connect(self.presenter.action_show_in_explorer)

        separator = QAction(self)
        separator.setSeparator(True)

        self.addAction(show_in_explorer)

    def setup_options_actions(self, parent):
        """
        Setup the actions for the Options menu. These are handled by the MultiFileInterpreter
        """
        # menu_bar = QMenuBar(self)
        # menu_bar.setStyleSheet("""border: 1px solid #adadad; background-color: #e1e1e1;""")
        options_button = QToolButton(self)
        self.setCornerWidget(options_button, Qt.TopRightCorner)
        options_button.setIcon(get_icon("fa.cog"))
        # options_button.setText("Options")
        options_menu = QMenu("Options", self)
        # menu_bar.addMenu(options_menu)
        options_button.clicked.connect(lambda: options_menu.popup(
            self.mapToGlobal(options_button.pos() + QPoint(0, options_button.rect().bottom()))))

        self.tabCloseRequested.connect(parent.close_tab)

        run_action = create_action(self, "Run", on_triggered=parent.execute_current,
                                   shortcut=("Ctrl+Return", "Ctrl+Enter"),
                                   shortcut_context=Qt.ApplicationShortcut)

        abort_action = create_action(self, "Abort", on_triggered=parent.abort_current)

        # menu action to toggle the find/replace dialog
        toggle_find_replace = create_action(self, 'Find/Replace...',
                                            on_triggered=parent.toggle_find_replace_dialog,
                                            shortcut='Ctrl+F')

        toggle_comment_action = create_action(self, "Comment/Uncomment", on_triggered=parent.toggle_comment_current,
                                              shortcut="Ctrl+/",
                                              shortcut_context=Qt.ApplicationShortcut)

        tabs_to_spaces_action = create_action(self, 'Tabs to Spaces', on_triggered=parent.tabs_to_spaces_current)

        spaces_to_tabs_action = create_action(self, 'Spaces to Tabs', on_triggered=parent.spaces_to_tabs_current)

        toggle_whitespace_action = create_action(self, 'Toggle Whitespace Visible',
                                                 on_triggered=parent.toggle_whitespace_visible_all)

        # Store actions for adding to menu bar; None will add a separator
        editor_actions = [run_action,
                          abort_action,
                          None,
                          toggle_find_replace,
                          None,
                          toggle_comment_action,
                          toggle_whitespace_action,
                          None,
                          tabs_to_spaces_action,
                          spaces_to_tabs_action]

        add_actions(options_menu, editor_actions)

    def closeEvent(self, event):
        self.deleteLater()
        super(CodeEditorTabWidget, self).closeEvent(event)

    def mousePressEvent(self, event):
        if Qt.MiddleButton == event.button():
            self.tabCloseRequested.emit(self.last_tab_clicked)

        QTabWidget(self).mousePressEvent(event)

    def tab_was_clicked(self, tab_index):
        self.last_tab_clicked = tab_index
