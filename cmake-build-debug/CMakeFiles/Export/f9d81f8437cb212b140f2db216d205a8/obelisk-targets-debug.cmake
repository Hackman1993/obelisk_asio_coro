#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "obelisk::obelisk" for configuration "Debug"
set_property(TARGET obelisk::obelisk APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(obelisk::obelisk PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/obelisk.lib"
  )

list(APPEND _cmake_import_check_targets obelisk::obelisk )
list(APPEND _cmake_import_check_files_for_obelisk::obelisk "${_IMPORT_PREFIX}/lib/obelisk.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
