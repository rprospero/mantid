#include "MantidQtWidgets/MplCpp//MplFigureCanvas.h"
#include "MantidQtWidgets/MplCpp//NDArray1D.h"
#include "MantidQtWidgets/MplCpp/PythonErrors.h"
#include "MantidQtWidgets/MplCpp/SipUtils.h"
#include "MantidQtWidgets/Common/PythonThreading.h"

#include <QVBoxLayout>

namespace MantidQt {
namespace Widgets {
namespace MplCpp {

namespace {
//------------------------------------------------------------------------------
// Static constants/functions
//------------------------------------------------------------------------------
#if QT_VERSION >= QT_VERSION_CHECK(4, 0, 0) &&                                 \
    QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
// Define PyQt version and matplotlib backend
const char *PYQT_MODULE = "PyQt4";
const char *MPL_QT_BACKEND = "matplotlib.backends.backend_qt4agg";

#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) &&                               \
    QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
// Define PyQt version and matplotlib backend
const char *PYQT_MODULE = "PyQt5";
const char *MPL_QT_BACKEND = "matplotlib.backends.backend_qt5agg";

#else
#error "Unknown Qt version. Cannot determine matplotlib backend."
#endif

// Return static instance of figure type
// The GIL must be held to call this
const PythonObject &mplFigureType() {
  static PythonObject figureType;
  if (figureType.isNone()) {
    figureType = getAttrOnModule("matplotlib.figure", "Figure");
  }
  return figureType;
}

// Return static instance of figure canvas type
// The GIL must be held to call this
const PythonObject &mplFigureCanvasType() {
  static PythonObject figureCanvasType;
  if (figureCanvasType.isNone()) {
    // Importing PyQt version first helps matplotlib select the correct backend.
    // We should do this in some kind of initialisation routine
    importModule(PYQT_MODULE);
    figureCanvasType = getAttrOnModule(MPL_QT_BACKEND, "FigureCanvasQTAgg");
  }
  return figureCanvasType;
}
}

//------------------------------------------------------------------------------
// MplFigureCanvas::PyObjectHolder - Private implementation
//------------------------------------------------------------------------------
struct MplFigureCanvas::PyObjectHolder {
  // QtAgg canvas object
  PythonObject canvas;
  // List of lines on current plot
  std::vector<PythonObject> lines;

  // constructor
  PyObjectHolder(int subplotLayout) {
    // Create a figure and attach it to a canvas object. This creates a
    // blank widget
    PythonObject figure(
        NewRef(PyObject_CallObject(mplFigureType().get(), NULL)));
    PythonObject(
        NewRef(PyObject_CallMethod(figure.get(), PYSTR_LITERAL("add_subplot"),
                                   PYSTR_LITERAL("i"), subplotLayout)));
    auto instance = PyObject_CallFunction(mplFigureCanvasType().get(),
                                          PYSTR_LITERAL("(O)"), figure.get());
    if (!instance) {
      throw PythonError(errorToString());
    }
    canvas = PythonObject(NewRef(instance));
  }

  /**
   * Return the Axes object that is currently active. Analogous to figure.gca()
   * @return matplotlib.axes.Axes object
   */
  PythonObject gca() {
    ScopedPythonGIL gil;
    auto figure = PythonObject(
        NewRef(PyObject_GetAttrString(canvas.get(), PYSTR_LITERAL("figure"))));
    return PythonObject(NewRef(PyObject_CallMethod(
        figure.get(), PYSTR_LITERAL("gca"), PYSTR_LITERAL(""), nullptr)));
  }
};

//------------------------------------------------------------------------------
// MplFigureCanvas
//------------------------------------------------------------------------------
/**
 * @brief Constructs an empty plot widget with the given subplot layout.
 *
 * @param subplotLayout The sublayout geometry defined in matplotlib's
 * convenience format: [Default=111]. See
 * https://matplotlib.org/api/pyplot_api.html#matplotlib.pyplot.subplot
 *
 * @param parent A pointer to the parent widget, can be nullptr
 */
MplFigureCanvas::MplFigureCanvas(int subplotLayout, QWidget *parent)
    : QWidget(parent), m_pydata(nullptr) {
  setLayout(new QVBoxLayout);

  QWidget *cpp(nullptr);
  { // acquire GIL
    ScopedPythonGIL gil;
    m_pydata = new PyObjectHolder(subplotLayout);
    cpp = static_cast<QWidget *>(sipUnwrap(m_pydata->canvas.get()));
  } // release GIL

  assert(cpp);
  layout()->addWidget(cpp);
}

/**
 * @brief Destroys the object
 */
MplFigureCanvas::~MplFigureCanvas() { delete m_pydata; }

/**
 * Retrieve information about the subplot geometry
 * @return A SubPlotSpec object defining the geometry
 */
SubPlotSpec MplFigureCanvas::getGeometry() const {
  ScopedPythonGIL gil;
  auto axes = m_pydata->gca();
  auto geometry = PythonObject(NewRef(PyObject_CallMethod(
      axes.get(), PYSTR_LITERAL("get_geometry"), PYSTR_LITERAL(""), nullptr)));

  return SubPlotSpec(PyInt_AS_LONG(PyTuple_GET_ITEM(geometry.get(), 0)),
                     PyInt_AS_LONG(PyTuple_GET_ITEM(geometry.get(), 1)));
}

/**
 * @return The number of Line2Ds on the canvas
 */
size_t MplFigureCanvas::nlines() const {
  ScopedPythonGIL gil;
  auto axes = m_pydata->gca();
  auto lines = PythonObject(NewRef(PyObject_CallMethod(
      axes.get(), PYSTR_LITERAL("get_lines"), PYSTR_LITERAL(""), nullptr)));
  return static_cast<size_t>(PyList_Size(lines.get()));
}

/**
 * Get a label from the canvas
 * @param type The label type
 * @return The label on the requested axis
 */
QString MplFigureCanvas::getLabel(const Axes::Label type) const {
  const char *method;
  if (type == Axes::Label::X)
    method = "get_xlabel";
  else if (type == Axes::Label::Y)
    method = "get_ylabel";
  else if (type == Axes::Label::Title)
    method = "get_title";
  else
    throw std::logic_error("MplFigureCanvas::getLabel() - Unknown label type.");

  ScopedPythonGIL gil;
  auto axes = m_pydata->gca();
  auto label = PythonObject(NewRef(PyObject_CallMethod(
      axes.get(), PYSTR_LITERAL(method), PYSTR_LITERAL(""), nullptr)));
  return QString::fromAscii(PyString_AsString(label.get()));
}

/**
 * Get the value of the requested scale
 * @param type Scale type to retrieve
 * @return A tuple of (min,max)
 */
std::tuple<double, double>
MplFigureCanvas::getScale(const Axes::Scale type) const {
  const char *method;
  if (type == Axes::Scale::X)
    method = "get_xlim";
  else if (type == Axes::Scale::Y)
    method = "get_ylim";
  else
    throw std::logic_error("MplFigureCanvas::getScale() - Unknown scale type.");

  ScopedPythonGIL gil;
  auto axes = m_pydata->gca();
  auto scale = PythonObject(NewRef(PyObject_CallMethod(
      axes.get(), PYSTR_LITERAL(method), PYSTR_LITERAL(""), nullptr)));
  return std::make_tuple(PyFloat_AsDouble(PyTuple_GetItem(scale.get(), 0)),
                         PyFloat_AsDouble(PyTuple_GetItem(scale.get(), 1)));
}

/**
 * Equivalent of Figure.add_subplot. If the subplot already exists then
 * it simply sets that plot number to be active
 * @param subplotLayout Subplot geometry in matplotlib convenience format,
 * e.g 2,1,2 would stack 2 plots on top of each other and set the second
 * to active
 */
void MplFigureCanvas::addSubPlot(int subplotLayout) {
  ScopedPythonGIL gil;
  auto figure = PythonObject(NewRef(
      PyObject_GetAttrString(m_pydata->canvas.get(), PYSTR_LITERAL("figure"))));
  auto result = PyObject_CallMethod(figure.get(), PYSTR_LITERAL("add_subplot"),
                                    PYSTR_LITERAL("(i)"), subplotLayout);
  if (!result)
    throw PythonError(errorToString());
  detail::decref(result);
}

/**
 * Plot lines to the current axis
 * @param x A container of X points. Requires support for forward iteration.
 * @param y A container of Y points. Requires support for forward iteration.
 * @param format A format string for the line/markers
 */
template <typename XArrayType, typename YArrayType>
void MplFigureCanvas::plotLine(const XArrayType &x, const YArrayType &y,
                               const char *format) {
  ScopedPythonGIL gil;
  NDArray1D xnp(x), ynp(y);
  auto axes = m_pydata->gca();
  // This will return a list of lines but we know we are only plotting 1
  auto lines =
      PyObject_CallMethod(axes.get(), PYSTR_LITERAL("plot"),
                          PYSTR_LITERAL("(OOs)"), xnp.get(), ynp.get(), format);
  if (!lines) {
    throw PythonError(errorToString());
  }
  m_pydata->lines.emplace_back(PythonObject(NewRef(PyList_GetItem(lines, 0))));
  detail::decref(lines);
}

/**
 * Remove a line from the canvas based on the index
 * @param index The index of the line to remove. If it does not exist then
 * this is a no-op.
 */
void MplFigureCanvas::removeLine(const size_t index) {
  ScopedPythonGIL gil;
  auto &lines = m_pydata->lines;
  if (lines.empty() || index >= lines.size())
    return;
  auto posIter = std::next(std::begin(lines), index);
  auto line = *posIter;
  lines.erase(posIter);
  PythonObject(NewRef(PyObject_CallMethod(line.get(), PYSTR_LITERAL("remove"),
                                          PYSTR_LITERAL(""), nullptr)));
}

/**
 * Clear the current axes of artists
 */
void MplFigureCanvas::clearLines() {
  if (m_pydata->lines.empty())
    return;
  ScopedPythonGIL gil;
  auto axes = m_pydata->gca();
  PythonObject(NewRef(PyObject_CallMethod(axes.get(), PYSTR_LITERAL("clear"),
                                          PYSTR_LITERAL(""), nullptr)));
  m_pydata->lines.clear();
}

/**
 * Set a label on the requested axis
 * @param type Type of label
 * @param label Label for the axis
 */
void MplFigureCanvas::setLabel(const Axes::Label type, const char *label) {
  const char *method;
  if (type == Axes::Label::X)
    method = "set_xlabel";
  else if (type == Axes::Label::Y)
    method = "set_ylabel";
  else if (type == Axes::Label::Title)
    method = "set_title";
  else
    throw std::logic_error("MplFigureCanvas::setLabel() - Unknown label type.");

  ScopedPythonGIL gil;
  auto axes = m_pydata->gca();
  auto result = PyObject_CallMethod(axes.get(), PYSTR_LITERAL(method),
                                    PYSTR_LITERAL("(s)"), label);
  if (!result)
    throw PythonError(errorToString());
  detail::decref(result);
}

/**
 * Set the requested scale to the given range
 * @param type The scale given by an Axes::Scale type
 * @param min Minimum value
 * @param max Maximum value
 */
void MplFigureCanvas::setScale(const Axes::Scale type, double min, double max) {
  const char *method;
  if (type == Axes::Scale::X)
    method = "set_xlim";
  else if (type == Axes::Scale::Y)
    method = "set_ylim";
  else
    throw std::logic_error("MplFigureCanvas::setScale() - Unknown scale type.");

  ScopedPythonGIL gil;
  auto axes = m_pydata->gca();
  auto result = PyObject_CallMethod(axes.get(), PYSTR_LITERAL(method),
                                    PYSTR_LITERAL("(dd)"), min, max);
  if (!result)
    throw PythonError(errorToString());
  detail::decref(result);
}

//------------------------------------------------------------------------------
// Explicit template instantations
//------------------------------------------------------------------------------
using VectorDouble = std::vector<double>;
template void MplFigureCanvas::plotLine<VectorDouble, VectorDouble>(
    const VectorDouble &, const VectorDouble &, const char *);
}
}
}
