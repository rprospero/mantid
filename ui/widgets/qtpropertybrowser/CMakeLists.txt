# Bring in Qt3support macros
include(${QT_USE_FILE})

# Add the sources
set ( SRC_FILES
  src/qtpropertybrowser.cpp
  src/qtvariantproperty.cpp
  src/qtbuttonpropertybrowser.cpp
  src/qtgroupboxpropertybrowser.cpp
  src/qtpropertybrowserutils.cpp
  src/DoubleEditorFactory.cpp
  src/ParameterPropertyManager.cpp
)

set ( MOC_SRC_FILES
  src/qttreepropertybrowser.cpp
  src/qteditorfactory.cpp
  src/qtpropertymanager.cpp
)

# These .moc files are included directly by the .cpp files themselves
# but still passed to add_library so it can set up the correct build dependencies
set ( MOC_GENERATED_FILES )
foreach ( _cpp ${MOC_SRC_FILES} )
  get_filename_component ( _filename_we ${_cpp} NAME_WE )
  set ( _generated_moc ${CMAKE_CURRENT_BINARY_DIR}/${_filename_we}.moc )
  set_source_files_properties ( ${_generated_moc} PROPERTIES HEADER_FILE_ONLY true )
  qt4_generate_moc ( ${_cpp} ${_generated_moc} )
  set ( MOC_GENERATED_FILES ${MOC_GENERATED_FILES} ${_generated_moc} )
endforeach ()

set ( MOC_HDR_FILES
  src/qtpropertybrowser.h
  src/qtpropertymanager.h
  src/qteditorfactory.h
  src/qtvariantproperty.h
  src/qttreepropertybrowser.h
  src/qtbuttonpropertybrowser.h
  src/qtgroupboxpropertybrowser.h
  src/qtpropertybrowserutils_p.h
  src/DoubleEditorFactory.h
  src/ButtonEditorFactory.h
  src/ParameterPropertyManager.h
)

# Need to use a string of calls to the qt4_generate_moc macro
# instead of qt4_wrap_cpp because the latter produces files with
# a cxx suffix when QtPropertyBrowser requires cpp
foreach ( _hdr ${MOC_HDR_FILES} )
  get_filename_component ( _filename_we ${_hdr} NAME_WE )
  set ( _generated_cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_${_filename_we}.cpp )
  set_source_files_properties ( ${_generated_cpp} PROPERTIES HEADER_FILE_ONLY true )
  qt4_generate_moc ( ${_hdr} ${_generated_cpp} )
  set ( MOC_GENERATED_FILES ${MOC_GENERATED_FILES} ${_generated_cpp} )
endforeach ()

# moc'd files will end up in build directory, so add to include path
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

qt4_add_resources (
  RESOURCES
  src/qtpropertybrowser.qrc
)

set ( TARGET_NAME QtPropertyBrowser )
add_library ( ${TARGET_NAME} SHARED ${SRC_FILES} ${MOC_SRC_FILES} ${MOC_HDR_FILES} ${MOC_GENERATED_FILES} ${RESOURCES} )

if (WIN32 OR CMAKE_COMPILER_IS_GNUCXX)
  target_compile_definitions ( ${TARGET_NAME} PUBLIC "QT_QTPROPERTYBROWSER_EXPORT" )
endif()

if (OSX_VERSION VERSION_GREATER 10.8)
  set_target_properties ( ${TARGET_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../MacOS")
endif ()

target_link_libraries ( ${TARGET_NAME} LINK_PRIVATE ${TCMALLOC_LIBRARIES_LINKTIME} ${QT_LIBRARIES} )

# This is, for the most part, not our code so disable an Intel compiler
# warning that crops up a lot in this package
if ( ${CMAKE_CXX_COMPILER_ID} MATCHES "Intel" )
  target_compile_definitions ( ${TARGET_NAME} PUBLIC "-wd1125" )
# Also make sure the equivalent gcc warning is turned off
elseif ( ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" )
  target_compile_options ( ${TARGET_NAME} PUBLIC "-Wno-overloaded-virtual" PUBLIC "-Wno-unused-but-set-variable" )
endif ()

###########################################################################
# Installation settings
###########################################################################

install ( TARGETS ${TARGET_NAME} DESTINATION ${LIB_DIR} )
