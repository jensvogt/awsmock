#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mongoc::static" for configuration "RelWithDebInfo"
set_property(TARGET mongoc::static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(mongoc::static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libmongoc2.a"
  )

list(APPEND _cmake_import_check_targets mongoc::static )
list(APPEND _cmake_import_check_files_for_mongoc::static "${_IMPORT_PREFIX}/lib/libmongoc2.a" )

# Import target "mongoc::shared" for configuration "RelWithDebInfo"
set_property(TARGET mongoc::shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(mongoc::shared PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libmongoc2.so.2.2.0"
  IMPORTED_SONAME_RELWITHDEBINFO "libmongoc2.so.2"
  )

list(APPEND _cmake_import_check_targets mongoc::shared )
list(APPEND _cmake_import_check_files_for_mongoc::shared "${_IMPORT_PREFIX}/lib/libmongoc2.so.2.2.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
