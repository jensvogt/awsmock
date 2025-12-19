# ReadVersion.cmake
# Reads version.txt and sets VERSION_STRING and VERSION_LIST

file(READ "${CMAKE_SOURCE_DIR}/version.txt" VERSION_STRING_RAW)
string(STRIP "${VERSION_STRING_RAW}" VERSION_STRING)

# Split version into major/minor/patch
string(REPLACE "." ";" VERSION_LIST "${VERSION_STRING}")
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)

message(STATUS "Project version: ${VERSION_STRING}")