if ( CXXTEST_FOUND )
  include_directories ( SYSTEM ${CXXTEST_INCLUDE_DIR} ${GMOCK_INCLUDE_DIR} ${GTEST_INCLUDE_DIR} )

  include_directories( ${TESTHELPERS_INCLUDE_DIR} ${FRAMEWORK_DIR}/DataObjects/inc ${FRAMEWORK_DIR}/Crystal/inc )

  set ( TESTHELPER_SRCS ${TESTHELPERS_SRC_DIR}/ComponentCreationHelper.cpp
                        ${TESTHELPERS_SRC_DIR}/InstrumentCreationHelper.cpp
                        ${TESTHELPERS_SRC_DIR}/WorkspaceCreationHelper.cpp
      )

  cxxtest_add_test ( MantidUiWidgetsCoreTest ${TEST_FILES} )
  target_link_libraries( MantidUiWidgetsCoreTest LINK_PRIVATE ${TCMALLOC_LIBRARIES_LINKTIME}
            MantidUiWidgetsCore
            QtPropertyBrowser
            ${CORE_MANTIDLIBS}
            DataObjects
            Crystal
            ${QT_LIBRARIES} ${QWT_LIBRARIES}
            ${Boost_LIBRARIES}
            ${POCO_LIBRARIES}
            ${PYTHON_LIBRARIES}
            ${GMOCK_LIBRARIES}
            ${GTEST_LIBRARIES}
            )

  # Add to the 'UnitTests' group in VS
  set_property( TARGET MantidUiWidgetsCoreTest PROPERTY FOLDER "UnitTests" )
endif ()
