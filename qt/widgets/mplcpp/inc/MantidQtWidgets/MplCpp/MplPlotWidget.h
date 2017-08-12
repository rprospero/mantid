#ifndef MPLPLOTWIDGET_H
#define MPLPLOTWIDGET_H

#include "MplFigureCanvas.h"

namespace MantidQt {
namespace Widgets {
namespace MplCpp {

/**
 * @brief High-level widget that contains a C++ matplotlib Qt canvas plus
 * optional toolbar.
 *
 * Most users will want to interact with this class instead of the lower level
 * classes. There are convenience functions defined to add high level items
 * instead of having to constantly access the Axes class.
 */
class EXPORT_OPT_MANTIDQT_MPLCPP MplPlotWidget : public QWidget {
  Q_OBJECT
public:
  MplPlotWidget(int subplotLayout = 111, QWidget *parent = nullptr);

  /// Access the canvas object
  /// @return A reference to the embedded canvas
  inline MplFigureCanvas &canvas() const { return *m_canvas; }

private:
  MplFigureCanvas *m_canvas;
};
}
}
}
#endif // MPLPLOTWIDGET_H
