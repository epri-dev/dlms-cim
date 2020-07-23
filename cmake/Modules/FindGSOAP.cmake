#
# This module detects if gsoap is installed and determines where the
# include files and libraries are.
#
# This code sets the following variables:
#
# GSOAP_IMPORT_DIR      = full path to the gsoap import directory
# GSOAP_CUSTOM_DIR      = full path to the gsoap custom directory
# GSOAP_LIBRARIES       = full path to the gsoap libraries
# GSOAP_SSL_LIBRARIES   = full path to the gsoap ssl libraries
# GSOAP_INCLUDE_DIR     = include dir to be used when using the gsoap library
# GSOAP_WSDL2H          = wsdl2h binary
# GSOAP_SOAPCPP2        = soapcpp2 binary
# GSOAP_FOUND           = set to true if gsoap was found successfully
#
# GSOAP_ROOT
#   setting this enables search for gsoap libraries / headers in this location

# -----------------------------------------------------
# GSOAP Import Directories
# -----------------------------------------------------
set(_x86 "(x86)")
file(GLOB _GSOAP_INCLUDE_DIRS
    "$ENV{ProgramFiles}/gsoap/*/include"
    "$ENV{ProgramFiles${_x86}}/gsoap/*/include"
    )
unset(_x86)

find_path(GSOAP_IMPORT_DIR
  NAMES wsp_appliesto.h
  PATHS 
    /usr/share/gsoap/import
    /usr/local/share/gsoap/import
    ${GSOAP_ROOT}/import 
    ${GSOAP_ROOT}/share/gsoap/import
)

find_path(GSOAP_CUSTOM_DIR
  NAMES qbytearray_hex.cpp
  PATHS 
    /usr/share/gsoap/custom
    /usr/local/share/gsoap/custom
    ${GSOAP_ROOT}/custom 
    ${GSOAP_ROOT}/share/gsoap/custom
)

# -----------------------------------------------------
# GSOAP Libraries
# -----------------------------------------------------
find_library(GSOAP_CXX_LIBRARIES
	NAMES gsoap++
	HINTS 
            /usr/lib64
            /usr/lib
            ${GSOAP_ROOT}/lib 
            ${GSOAP_ROOT}/lib64
            ${GSOAP_ROOT}/lib32
)
find_library(GSOAP_SSL_CXX_LIBRARIES
	NAMES gsoapssl++
	HINTS 
            /usr/lib64
            /usr/lib
            ${GSOAP_ROOT}/lib 
            ${GSOAP_ROOT}/lib64
            ${GSOAP_ROOT}/lib32
)

find_library(GSOAP_C_LIBRARIES
	NAMES gsoap
	HINTS 
            /usr/lib64
            /usr/lib
            ${GSOAP_ROOT}/lib 
            ${GSOAP_ROOT}/lib64
            ${GSOAP_ROOT}/lib32
)
find_library(GSOAP_SSL_C_LIBRARIES
	NAMES gsoapssl
	HINTS 
            /usr/lib64
            /usr/lib
            ${GSOAP_ROOT}/lib 
            ${GSOAP_ROOT}/lib64
            ${GSOAP_ROOT}/lib32
)

# -----------------------------------------------------
# GSOAP Include Directories
# -----------------------------------------------------
find_path(GSOAP_INCLUDE_DIR
	NAMES stdsoap2.h
	HINTS 
            /usr/include
            /usr/local/include
            ${GSOAP_ROOT} 
            ${GSOAP_ROOT}/include 
            ${GSOAP_ROOT}/include/*
)

# -----------------------------------------------------
# GSOAP Binaries
# -----------------------------------------------------
if(NOT GSOAP_TOOL_DIR)
	set(GSOAP_TOOL_DIR GSOAP_ROOT)
endif()

find_program(GSOAP_WSDL2H
	NAMES wsdl2h
	HINTS 
            /usr/bin
            /usr/local/bin
            ${GSOAP_TOOL_DIR}/bin
)
find_program(GSOAP_SOAPCPP2
	NAMES soapcpp2
        HINTS
            /usr/bin
            /usr/local/bin
            ${GSOAP_TOOL_DIR}/bin
)

# -----------------------------------------------------
# GSOAP version
# -----------------------------------------------------
execute_process(COMMAND ${GSOAP_SOAPCPP2}  "-V"   OUTPUT_VARIABLE GSOAP_STRING_VERSION ERROR_VARIABLE GSOAP_STRING_VERSION )
string(REGEX MATCH "[0-9]*\\.[0-9]*\\.[0-9]*" GSOAP_VERSION ${GSOAP_STRING_VERSION})

## TODO: add code to check for old versions

# -----------------------------------------------------
# Handle standard args
# -----------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    GSOAP 
    DEFAULT_MSG 
    GSOAP_CXX_LIBRARIES 
    GSOAP_C_LIBRARIES
    GSOAP_INCLUDE_DIR 
    GSOAP_WSDL2H 
    GSOAP_SOAPCPP2
)
mark_as_advanced(
    GSOAP_INCLUDE_DIR 
    GSOAP_LIBRARIES 
    GSOAP_WSDL2H 
    GSOAP_SOAPCPP2
)
