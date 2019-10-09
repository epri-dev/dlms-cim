cmake_minimum_required(VERSION 3.1)
project(dlms-cim)
set(dlms_cim_MAJOR_VERSION 1)
set(dlms_cim_MINOR_VERSION 0)
set(dlms_cim_MICRO_VERSION 0)
set(dlms_cim_VERSION ${dlms_cim_MAJOR_VERSION}.${dlms_cim_MINOR_VERSION}.${dlms_cim_MICRO_VERSION})
set(CMAKE_CXX_STANDARD 11)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/DLMS_sim/dlms-cimConfig.h.in"
  "${PROJECT_BINARY_DIR}/dlms-cimConfig.h"
  )

# add the binary tree to the search path for include files
# so that we will find dlms-cimConfig.h
include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory(DLMS-COSEM)
add_subdirectory(DLMS_sim)
