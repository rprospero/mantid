#ifndef MPLFIGURECANVAS_H
#define MPLFIGURECANVAS_H
#include "MantidQtWidgets/MplCpp/DllOption.h"
#include <QWidget>

namespace MantidQt {
namespace Widgets {
namespace MplCpp {

struct EXPORT_OPT_MANTIDQT_MPLCPP SubPlotSpec {
  SubPlotSpec(long rows, long cols) : nrows(rows), ncols(cols) {}
  // These are long to match Python ints so we can avoid some casts
  long nrows;
  long ncols;
};

/**
 * @brief Non-member equality operator
 * @param lhs Left-hand side object
 * @param rhs Right-hand side object
 * @return True if the number of rows/cols match
 */
inline bool operator==(const SubPlotSpec &lhs, const SubPlotSpec &rhs) {
  return lhs.nrows == rhs.nrows && lhs.ncols == rhs.ncols;
}

/**
 * C++ wrapper of a matplotlib backend FigureCanvas
 *
 * It uses the Agg version of the Qt matplotlib backend that matches the
 * Qt version that the library is compiled against.
 *
 * This replicates the example of embedding in Python from
 * https://matplotlib.org/examples/user_interfaces/embedding_in_qt5.html
 */
class EXPORT_OPT_MANTIDQT_MPLCPP MplFigureCanvas : public QWidget {
  Q_OBJECT
public:
  MplFigureCanvas(int subplotLayout = 111, QWidget *parent = 0);
  ~MplFigureCanvas();

  ///@{
  ///@name Query properties
  SubPlotSpec getGeometry() const;
  size_t nlines() const;
  ///@}

  ///@{
  ///@name Modify the canvas
  void addSubPlot(int subplotLayout);
  template <typename XArrayType, typename YArrayType>
  void plotLine(const XArrayType &x, const YArrayType &y, const char *format);
  void removeLine(const size_t index);
  ///@}

private:
  // Python objects are held in an hidden type to avoid Python
  // polluting the header
  struct PyObjectHolder;
  PyObjectHolder *m_pydata;
};
}
}
}
#endif // MPLFIGURECANVAS_H
