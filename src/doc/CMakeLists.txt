find_package(Doxygen)
set(MD_SOURCES intro.md design.md using.md building.md)
if(DOXYGEN_FOUND)
    configure_file(
        "${dlms-cim_SOURCE_DIR}/src/doc/doxygen.conf.in"
        "${CMAKE_BINARY_DIR}/doxygen.conf"
        @ONLY
    )
    configure_file(
        "${dlms-cim_SOURCE_DIR}/src/doc/epri-logo.png"
        "${CMAKE_BINARY_DIR}/doc/epri-logo.png"
        COPYONLY
    )
    configure_file(
        "${dlms-cim_SOURCE_DIR}/src/doc/header.tex"
        "${CMAKE_BINARY_DIR}/doc/header.tex"
        COPYONLY
    )
    add_custom_target(
        doc
        "${DOXYGEN_EXECUTABLE}"
        "${CMAKE_BINARY_DIR}/doxygen.conf"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        DEPENDS "${MD_SOURCES}"
        COMMENT "Generating API Reference documentation..." VERBATIM
    )
    add_custom_target(
        pdf
        DEPENDS "${CMAKE_BINARY_DIR}/doc/latex/refman.pdf"
    )
    add_custom_command(
        OUTPUT  "${CMAKE_BINARY_DIR}/doc/latex/refman.pdf"
        COMMAND "make"
        DEPENDS "doc"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/doc/latex"
        COMMENT "Generating PDF format Reference documentation..." VERBATIM
    )

endif(DOXYGEN_FOUND)
