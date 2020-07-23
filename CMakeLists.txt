cmake_minimum_required(VERSION 3.1)
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(dlms-cim
    VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 11)

# load additional detection for GSOAP
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/src/dlms-cimConfig.h.in"
  "${PROJECT_BINARY_DIR}/dlms-cimConfig.h"
  )

# add the binary tree to the search path for include files
# so that we will find dlms-cimConfig.h
include_directories("${PROJECT_BINARY_DIR}")

add_subdirectory(DLMS-COSEM)
add_subdirectory(src)
SET(CPACK_SOURCE_IGNORE_FILES "/build/;.swp;.git")
SET(CPACK_PACKAGE_VENDOR "EPRI")
set(CPACK_SOURCE_GENERATOR "TGZ")
INCLUDE(InstallRequiredSystemLibraries)
include (CPack)
