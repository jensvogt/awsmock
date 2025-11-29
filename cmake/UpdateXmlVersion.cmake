# ==============================================================================
# update_xml_version(<xml_file> <new_version> <tag>)
#
# Example:
#   update_xml_version(${CMAKE_SOURCE_DIR}/config.xml "1.2.3" "Version")
#
# Replaces:
#   <Version>...</Version>
# With:
#   <Version>1.2.3</Version>
#
# Works cross-platform (pure CMake).
# ==============================================================================

function(update_xml_version XML_FILE NEW_VERSION TAG_NAME)
    if (NOT EXISTS "${XML_FILE}")
        message(FATAL_ERROR "XML file not found: ${XML_FILE}")
    endif ()

    file(READ "${XML_FILE}" XML_CONTENTS)

    # Build the search & replacement patterns
    set(PATTERN "<${TAG_NAME}>[^<]*</${TAG_NAME}>")
    set(REPLACEMENT "<${TAG_NAME}>${NEW_VERSION}</${TAG_NAME}>")

    string(REGEX REPLACE "${PATTERN}" "${REPLACEMENT}" XML_CONTENTS "${XML_CONTENTS}")

    file(WRITE "${XML_FILE}" "${XML_CONTENTS}")

    message(STATUS "Updated <${TAG_NAME}> to version ${NEW_VERSION} in ${XML_FILE}")
endfunction()
