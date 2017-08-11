#include "MantidQtWidgets/MplCpp/MplPlotWidget.h"

#include <QVBoxLayout>

namespace MantidQt {
namespace Widgets {
namespace MplCpp {

/**
 * @brief Construct a plot widget without a toolbar with the canvas
 * having the given subplot layout
 * @param subplotLayout @see MplFigureCanvas
 * @param parent The parent widget
 */
MplPlotWidget::MplPlotWidget(int subplotLayout, QWidget *parent)
    : QWidget(parent), m_canvas(new MplFigureCanvas(subplotLayout, this)) {
  setLayout(new QVBoxLayout);
  layout()->addWidget(m_canvas);
}
}
}
}
