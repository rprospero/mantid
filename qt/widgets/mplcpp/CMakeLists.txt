# Rules for matplotlib cpp library
include (QtTargetFunctions)

set (LIB_SRCS
  src/MplFigureCanvas.cpp
  src/MplPlotWidget.cpp
  src/NDArray1D.cpp
  src/Numpy.cpp
  src/PythonErrors.cpp
  src/PythonObject.cpp
  src/SipUtils.cpp
)

set (MOC_HEADERS
  inc/MantidQtWidgets/MplCpp/MplFigureCanvas.h
  inc/MantidQtWidgets/MplCpp/MplPlotWidget.h
)

set (NOMOC_HEADERS
  inc/MantidQtWidgets/MplCpp/NDArray1D.h
  inc/MantidQtWidgets/MplCpp/Numpy.h
  inc/MantidQtWidgets/MplCpp/PythonErrors.h
  inc/MantidQtWidgets/MplCpp/PythonObject.h
  inc/MantidQtWidgets/MplCpp/SipUtils.h
)

set (INCLUDES
  ${CMAKE_CURRENT_LIST_DIR}/inc
  ${CMAKE_CURRENT_LIST_DIR}/../commmon/inc
)

# Target
include_directories (SYSTEM ${PYTHON_INCLUDE_DIR})
mtd_add_qt_library (TARGET_NAME MantidQtWidgetsMplCpp QT_VERSION 4 SRC ${LIB_SRCS} MOC ${MOC_HEADERS}
                    NOMOC ${NOMOC_HEADERS} INCLUDE_DIRS ${INCLUDES}
                    LINK_LIBRARIES MantidQtWidgetsCommon ${PYTHON_LIBRARIES})
set_target_properties (MantidQtWidgetsMplCpp-Qt4 PROPERTIES COMPILE_DEFINITIONS IN_MANTIDQT_MPLCPP)

# Tests
add_subdirectory (test)
