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
  MplPlotWidget(int subplotLayout, QWidget *parent = nullptr);

  inline void draw() { m_canvas->draw(); }
  inline void addSubPlot(int subplotLayout) {
    m_canvas->addSubPlot(subplotLayout);
  }

  template <typename XArrayType, typename YArrayType>
  inline void plot(XArrayType &&x, YArrayType &&y, const char *format) {
    m_canvas->plot(std::forward<XArrayType>(x), std::forward<YArrayType>(y),
                   format);
  }

private:
  MplFigureCanvas *m_canvas;
};
}
}
}
#endif // MPLPLOTWIDGET_H
