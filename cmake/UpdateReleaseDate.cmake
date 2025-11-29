# =============================================================================
# UpdateReleaseDate.cmake - Fixed version
# =============================================================================

function(update_release_date PACKAGE_XML_PATH)
    if (NOT EXISTS "${PACKAGE_XML_PATH}")
        message(FATAL_ERROR "Package XML not found: ${PACKAGE_XML_PATH}")
    endif ()

    # Get current date and time in YYYY-MM-DD HH:MM:SS
    string(TIMESTAMP CURRENT_DATETIME "%Y-%m-%d %H:%M:%S")
    message(STATUS "Current Datetime ${CURRENT_DATETIME}")

    # Read XML content
    file(READ "${PACKAGE_XML_PATH}" XML_CONTENT)

    # Check if <ReleaseDate> exists
    string(FIND "${XML_CONTENT}" "<ReleaseDate>" FOUND_POS)
    if (NOT FOUND_POS EQUAL -1)
        # Replace existing <ReleaseDate>...</ReleaseDate>
        string(REGEX REPLACE "<ReleaseDate>[^<]*</ReleaseDate>"
                "<ReleaseDate>${CURRENT_DATETIME}</ReleaseDate>" XML_CONTENT "${XML_CONTENT}")
    else ()
        # Insert <ReleaseDate> before closing </Package> tag
        string(REPLACE "</Package>" "<ReleaseDate>${CURRENT_DATETIME}</ReleaseDate>\n</Package>" XML_CONTENT "${XML_CONTENT}")
    endif ()

    # Write updated content back
    file(WRITE "${PACKAGE_XML_PATH}" "${XML_CONTENT}")

    message(STATUS "Updated ReleaseDate in ${PACKAGE_XML_PATH} to ${CURRENT_DATETIME}")
endfunction()
