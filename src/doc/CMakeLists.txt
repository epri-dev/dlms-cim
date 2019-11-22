find_package(Doxygen)
set(MD_SOURCES 01_intro.md 02_building.md 03_using.md 04_native.md 05_design.md )
set(IMAGE_PATH "images")
set(DEST_IMAGE_PATH "${CMAKE_BINARY_DIR}/${IMAGE_PATH}")
if(DOXYGEN_FOUND)
    configure_file(
        "${dlms-cim_SOURCE_DIR}/src/doc/doxygen.conf.in"
        "${CMAKE_BINARY_DIR}/doxygen.conf"
        @ONLY
    )
    configure_file(
        "${dlms-cim_SOURCE_DIR}/src/doc/epri-logo.png"
        "${DEST_IMAGE_PATH}/epri-logo.png"
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
        DEPENDS "${DEST_IMAGE_PATH}/Wireshark1.png"
        DEPENDS "${DEST_IMAGE_PATH}/blocks.svg"
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
        DEPENDS "${DEST_IMAGE_PATH}/blocks.pdf"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/doc/latex"
        COMMENT "Generating PDF format Reference documentation..." VERBATIM
    )
    add_custom_command(
        OUTPUT "${DEST_IMAGE_PATH}/Wireshark1.png" 
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_LIST_DIR}/${IMAGE_PATH}" "${DEST_IMAGE_PATH}"
        COMMENT "Copying image files..." VERBATIM
    )

    # There must be a better way to do this!
    add_custom_command(
        OUTPUT "${DEST_IMAGE_PATH}/blocks.svg" "${DEST_IMAGE_PATH}/blocks.pdf" 
        COMMAND ${DOXYGEN_DOT_EXECUTABLE}
            -Tsvg ${CMAKE_CURRENT_LIST_DIR}/blocks.dot -o${DEST_IMAGE_PATH}/blocks.svg 
            -Tpdf ${CMAKE_CURRENT_LIST_DIR}/blocks.dot -o${DEST_IMAGE_PATH}/blocks.pdf
        DEPENDS "blocks.dot"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Generating drawings using graphviz..." 
    )
endif(DOXYGEN_FOUND)
