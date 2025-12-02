#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "bson::shared" for configuration "RelWithDebInfo"
set_property(TARGET bson::shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(bson::shared PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libbson2.so.2.2.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libbson2.so.2"
  )

list(APPEND _cmake_import_check_targets bson::shared )
list(APPEND _cmake_import_check_files_for_bson::shared "${_IMPORT_PREFIX}/lib/libbson2.so.2.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
