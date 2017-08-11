# Unit tests
#
set (STD_TESTS
  PythonObjectTest.h
  NDArray1DTest.h
)

set (QT_TESTS
  MplFigureCanvasTest.h
)
# TESTHELPER_SRCS is used internally by cxxtest_add_test when set
qt4_wrap_cpp (TESTHELPER_SRCS ${QT_TESTS})

# CXXTEST_EXTRA_HEADER_INCLUDE passes this file to the --include argument
set (CXXTEST_EXTRA_HEADER_INCLUDE GlobalInitialization.h)

include_directories (SYSTEM ${CXXTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR}
                     ${GTEST_INCLUDE_DIR})
cxxtest_add_test (MantidQtWidgetsMplCppTest ${STD_TESTS} ${QT_TESTS})
target_include_directories (MantidQtWidgetsMplCppTest PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../inc)
target_link_libraries (MantidQtWidgetsMplCppTest Qt4::QtGui MantidQtWidgetsMplCpp-Qt4 ${PYTHON_LIBRARIES})
