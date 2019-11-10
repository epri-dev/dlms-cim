#
# This module detects if Asio is installed and determines where the
# include files and libraries are.
#
# This code sets the following variables:
#
# ASIO_INCLUDE_DIR     = include dir to be used when using the asio library
# ASIO_FOUND           = set to true if asio was found successfully
#
# ASIO_ROOT
#   setting this enables search for asio libraries / headers in this location

# -----------------------------------------------------
# ASIO Include Directories
# -----------------------------------------------------
set(_x86 "(x86)")
file(GLOB _Asio_INCLUDE_DIRS
    "$ENV{ProgramFiles}/asio/*/include"
    "$ENV{ProgramFiles${_x86}}/asio/*/include"
    )
unset(_x86)

find_path(ASIO_INCLUDE_DIR
    NAMES
        asio.hpp
    PATHS
        ${_Asio_INCLUDE_DIRS}
        /usr/include
        /usr/local/include
        ${ASIO_ROOT}
        ${ASIO_ROOT}/include
)

# -----------------------------------------------------
# ASIO version
# -----------------------------------------------------
if(ASIO_INCLUDE_DIR AND EXISTS "${ASIO_INCLUDE_DIR}/asio/version.hpp")
    file(STRINGS "${ASIO_INCLUDE_DIR}/asio/version.hpp" ASIO_H REGEX "^#define ASIO_VERSION [^/]*/")
    string(REGEX REPLACE "^.*ASIO_VERSION ([0-9]).*$" "\\1" ASIO_VERSION_MAJOR "${ASIO_H}")
    string(REGEX REPLACE "^.*ASIO_VERSION [0-9]([0-9][0-9][0-9]).*$" "\\1" ASIO_VERSION_MINOR "${ASIO_H}")
    string(REGEX REPLACE "^.*ASIO_VERSION [0-9][0-9][0-9][0-9]([0-9][0-9]).*$" "\\1" ASIO_VERSION_PATCH "${ASIO_H}")
    set(ASIO_VERSION "${ASIO_VERSION_MAJOR}.${ASIO_VERSION_MINOR}.${ASIO_VERSION_PATCH}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Asio
    DEFAULT_MSG
    ASIO_INCLUDE_DIR
)
