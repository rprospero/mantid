# Mantid Repository : https://github.com/mantidproject/mantid
#
# Copyright &copy; 2019 ISIS Rutherford Appleton Laboratory UKRI,
#     NScD Oak Ridge National Laboratory, European Spallation Source
#     & Institut Laue - Langevin
# SPDX - License - Identifier: GPL - 3.0 +
#  This file is part of the mantid workbench.

from __future__ import (absolute_import, unicode_literals)

from mantid.plots import MantidAxes

from mantidqt.widgets.plotconfigdialog import curve_in_ax
from workbench.plugins.editor import DEFAULT_CONTENT
from workbench.plotting.plotscriptgenerator.axes import (generate_axis_limit_commands,
                                                         generate_axis_label_commands,
                                                         generate_set_title_command)
from workbench.plotting.plotscriptgenerator.figure import generate_subplots_command
from workbench.plotting.plotscriptgenerator.lines import generate_plot_command
from workbench.plotting.plotscriptgenerator.utils import generate_workspace_retrieval_commands, sorted_lines_in

FIG_VARIABLE = "fig"
AXES_VARIABLE = "axes"


def generate_script(fig, exclude_headers=False):
    """
    Generate a script to recreate a figure.

    This currently only supports recreating artists that were plotted
    from a Workspace. The format of the outputted script is as follows:

        <Default Workbench script contents (imports)>
        <Workspace retrieval from ADS>
        fig, axes = plt.subplots()
        axes.plot() or axes.errorbar()
        ax.legend().draggable()     (if legend present)
        plt.show()

    :param fig: A matplotlib.pyplot.Figure object you want to create a script from
    :param exclude_headers: Boolean. Set to True to ignore imports/headers
    :return: A String. A script to recreate the given figure
    """
    plot_commands = []
    for ax in fig.get_axes():
        if not isinstance(ax, MantidAxes) or not curve_in_ax(ax):
            continue

        # plt.subplots returns an Axes object if there's only one axes being
        # plotted otherwise it returns a list
        ax_object_str = AXES_VARIABLE
        if ax.numRows > 1:
            ax_object_str += "[{row_num}]".format(row_num=ax.rowNum)
        if ax.numCols > 1:
            ax_object_str += "[{col_num}]".format(col_num=ax.colNum)

        for artist in sorted_lines_in(ax, ax.get_tracked_artists()):
            plot_commands.append("{ax_obj}.{cmd}".format(ax_obj=ax_object_str,
                                                         cmd=generate_plot_command(artist)))
        # Add command for setting title
        if ax.get_title():
            plot_commands.append("{ax_obj}.{cmd}".format(ax_obj=ax_object_str,
                                                         cmd=generate_set_title_command(ax)))
        # Get ax.set_x/ylim() commands
        axis_limit_cmds = generate_axis_limit_commands(ax)
        plot_commands += [
            "{ax_obj}.{cmd}".format(ax_obj=ax_object_str, cmd=cmd) for cmd in axis_limit_cmds
        ]
        # Get ax.set_x/ylabel() commands
        axis_label_cmds = generate_axis_label_commands(ax)
        plot_commands += [
            "{ax_obj}.{cmd}".format(ax_obj=ax_object_str, cmd=cmd) for cmd in axis_label_cmds
        ]

        if ax.legend_:
            plot_commands.append("{ax_obj}.legend().draggable()".format(ax_obj=ax_object_str))

        plot_commands.append('')

    if not plot_commands:
        return
    cmds = [] if exclude_headers else [DEFAULT_CONTENT]
    cmds += generate_workspace_retrieval_commands(fig) + ['']
    cmds.append("{}, {} = {}".format(FIG_VARIABLE, AXES_VARIABLE, generate_subplots_command(fig)))
    cmds += plot_commands
    cmds.append("plt.show()")
    return '\n'.join(cmds)


if __name__ == '__main__':
    import matplotlib.pyplot as plt
    import numpy as np
    from mantid.simpleapi import CreateWorkspace

    x = np.linspace(0, 10, 100)
    y = np.sin(x)
    e = np.sqrt(np.abs(y))

    ws = CreateWorkspace(x, y, DataE=e, OutputWorkspace="ws")
    fig, axes = plt.subplots(1, 1, subplot_kw={'projection': 'mantid'})
    # for ax in axes:
    #     ax.plot(ws)
    axes.plot(ws)

    print(generate_script(fig))
