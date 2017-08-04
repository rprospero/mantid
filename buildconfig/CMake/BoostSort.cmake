include(ExternalProject)

if ( Boost_VERSION VERSION_LESS 1.58 ) 
  # Download and unpack Boost Sort at configure time
  configure_file(${CMAKE_SOURCE_DIR}/buildconfig/CMake/BoostSort.in ${CMAKE_BINARY_DIR}/boost-sort-download/CMakeLists.txt)

  # The OLD behavior for this policy is to ignore the visibility properties
  # for static libraries, object libraries, and executables without exports.
  cmake_policy(SET CMP0063 "OLD")

  execute_process(COMMAND ${CMAKE_COMMAND} . WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/boost-sort-download )
  execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/boost-sort-download )

  set(BOOST_SORT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/boost-sort-src/include" CACHE PATH "Boost Sort include directory" FORCE)
  ## Include the source directory.
else ()
  set(BOOST_SORT_INCLUDE_DIR "${Boost_INCLUDE_DIRS}" CACHE PATH "Boost Sort include directory")
endif()
