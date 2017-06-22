################################################################################
# Components related to graphical interfaces
################################################################################

###########################################################################
# Qt
###########################################################################
find_package ( Qt4 COMPONENTS QtCore QtGui QtOpenGL QtXml QtSvg REQUIRED )

###########################################################################
# PyQt & sip
###########################################################################
find_package ( PyQt4 REQUIRED )
find_package ( SIP REQUIRED )
separate_arguments ( PYQT4_SIP_FLAGS )

###########################################################################
# Qwt Needs to be version 5
###########################################################################
find_package ( Qwt REQUIRED )
if( QWT_VERSION VERSION_LESS 5.0 OR QWT_VERSION VERSION_EQUAL 6.0 OR
      QWT_VERSION VERSION_GREATER 6.0 )
    message ( FATAL_ERROR "Qwt version 5 is required, found: ${QWT_VERSION}" )
endif()

if ( MSVC )
  add_definitions ( -DQWT_DLL )
endif ()

################################################################################
# Libraries, widgets etc
################################################################################

# Add a target for all GUI tests
add_custom_target ( GUITests )
add_dependencies ( check GUITests )
add_dependencies ( AllTests GUITests )

if ( MAKE_VATES )
  add_subdirectory ( vates )
  if ( NOT APPLE )
    LIST( APPEND CPACK_INSTALL_CMAKE_PROJECTS
          "${ParaView_DIR}" "ParaView Runtime Libs" "Runtime" "${INBUNDLE}/"
    )
    LIST( APPEND CPACK_INSTALL_CMAKE_PROJECTS
          "${ParaView_DIR}" "VTK Runtime Libs" "RuntimeLibraries" "${INBUNDLE}/"
    )
    LIST( APPEND CPACK_INSTALL_CMAKE_PROJECTS
          "${ParaView_DIR}" "HDF5 Core Library" "libraries" "${INBUNDLE}/"
    )
    LIST( APPEND CPACK_INSTALL_CMAKE_PROJECTS
          "${ParaView_DIR}" "HDF5 HL Library" "hllibraries" "${INBUNDLE}/"
    )
  endif ()
endif ()

add_subdirectory ( QtPropertyBrowser )
add_subdirectory ( MantidQt )

################################################################################
# User applications
################################################################################
add_subdirectory (applications/mantidplot)
