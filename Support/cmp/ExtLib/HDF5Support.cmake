
# -------------------------------------------------------------
# This function adds the necessary cmake code to find the HDF5
# shared libraries and setup custom copy commands and/or install
# rules for Linux and Windows to use
function(AddHDF5CopyInstallRules)
  set(options )
  set(oneValueArgs LIBNAME LIBVAR)
  set(multiValueArgs TYPES)
  cmake_parse_arguments(Z "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  set(INTER_DIR ".")

  #message(STATUS "Z_LIBNAME: ${Z_LIBNAME}")
  #message(STATUS "Z_LIBVAR: ${Z_LIBVAR}")
  #message(STATUS "Z_TYPES: ${Z_TYPES}")

  set(Z_INSTALL_DIR "lib")
  if(WIN32)
    set(Z_INSTALL_DIR ".")
  endif()

  FOREACH(BTYPE ${Z_TYPES} )
    #message(STATUS "BTYPE: ${BTYPE}")
    STRING(TOUPPER ${BTYPE} TYPE)
    if(MSVC_IDE)
      set(INTER_DIR "${BTYPE}")
    endif()

    # Get the Actual Library Path and create Install and copy rules
    GET_TARGET_PROPERTY(LibPath ${Z_LIBNAME} IMPORTED_LOCATION_${TYPE})
    #message(STATUS "LibPath: ${LibPath}")
    if(NOT "${LibPath}" STREQUAL "LibPath-NOTFOUND")
      #message(STATUS "Creating Install Rule for ${LibPath}")
      install(FILES ${LibPath}
        DESTINATION "${Z_INSTALL_DIR}"
        CONFIGURATIONS ${BTYPE}
        COMPONENT Applications)


      ADD_CUSTOM_TARGET(ZZ_${Z_LIBVAR}_DLL_${TYPE}-Copy ALL
                          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LibPath}
                          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/
                          COMMENT "  Copy: ${LibPath}\n    To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/")
      set_target_properties(ZZ_${Z_LIBVAR}_DLL_${TYPE}-Copy PROPERTIES FOLDER ZZ_COPY_FILES)

    endif()

    # Now get the path that the library is in
    GET_FILENAME_COMPONENT(${Z_LIBVAR}_DIR ${LibPath} PATH)
   # message(STATUS "${Z_LIBVAR}_DIR: ${${Z_LIBVAR}_DIR}")

    # Now piece together a complete path for the symlink that Linux Needs to have
    if(WIN32)
      GET_TARGET_PROPERTY(${Z_LIBVAR}_${TYPE} ${Z_LIBNAME} IMPORTED_IMPLIB_${TYPE})
    else()
      GET_TARGET_PROPERTY(${Z_LIBVAR}_${TYPE} ${Z_LIBNAME} IMPORTED_SONAME_${TYPE})
    endif()

    #message(STATUS "${Z_LIBVAR}_${TYPE}: ${${Z_LIBVAR}_${TYPE}}")
    if(NOT "${${Z_LIBVAR}_${TYPE}}" STREQUAL "${Z_LIBVAR}_${TYPE}-NOTFOUND" AND NOT WIN32)
      set(SYMLINK_PATH "${${Z_LIBVAR}_DIR}/${${Z_LIBVAR}_${TYPE}}")
      #message(STATUS "Creating Install Rule for ${SYMLINK_PATH}")
      install(FILES ${SYMLINK_PATH}
        DESTINATION "${Z_INSTALL_DIR}"
        CONFIGURATIONS ${BTYPE}
        COMPONENT Applications)

      ADD_CUSTOM_TARGET(ZZ_${Z_LIBVAR}_SYMLINK_${TYPE}-Copy ALL
                          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SYMLINK_PATH}
                          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/
                          COMMENT "  Copy: ${SYMLINK_PATH}\n    To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/")
      set_target_properties(ZZ_${Z_LIBVAR}_SYMLINK_${TYPE}-Copy PROPERTIES FOLDER ZZ_COPY_FILES)

    endif()

  endforeach()
endfunction()


#------------------------------------------------------------------------------
# Find HDF5 Headers/Libraries
# HDF5 now comes with everything that is needed for CMake to load
# up the targets (Exported) that it needs. We just need to find where HDF5 is installed.
#------------------------------------------------------------------------------
if("${HDF5_INSTALL}" STREQUAL "")
    set(HDF5_INSTALL  $ENV{HDF5_INSTALL})
endif()

if( "${HDF5_INSTALL}" STREQUAL "")
  message(FATAL_ERROR "The HDF5_INSTALL variable was not set. In order to find HDF5 you need to either\
                      pass in the -DHDF5_INSTALL=.... or set the HDF5_INSTALL environment variable.")
endif()

if(WIN32)
    set(ENV{HDF5_ROOT_DIR_HINT} "${HDF5_INSTALL}/cmake/hdf5")
    set(ENV{HDF5_ROOT} "${HDF5_INSTALL}")
    set(CMAKE_MODULE_PATH ${HDF5_INSTALL}/cmake/hdf5 ${CMAKE_MODULE_PATH})
else()
    set(ENV{HDF5_ROOT_DIR_HINT} "${HDF5_INSTALL}/share/cmake")
    set(ENV{HDF5_ROOT} "${HDF5_INSTALL}")
    set(CMAKE_MODULE_PATH ${HDF5_INSTALL}/share/cmake/hdf5 ${CMAKE_MODULE_PATH})
endif()

find_package(HDF5)
if(HDF5_FOUND)
  file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${HDF5_LIBRARY_DIRS};")
  include_directories(${HDF5_INCLUDE_DIRS} )
  list(APPEND CMP_LIB_SEARCH_DIRS ${HDF5_BINARY_DIR} ${HDF5_LIBRARY_DIR} )

  message(STATUS "HDF5 Location: ${HDF5_INSTALL}")
  message(STATUS "HDF5 Version: ${HDF5_VERSION_STRING}")
  if(MSVC_IDE)
    set(BUILD_TYPES Debug Release)
  else()
    set(BUILD_TYPES "${CMAKE_BUILD_TYPE}")
    if("${BUILD_TYPES}" STREQUAL "")
        set(BUILD_TYPES "Debug")
    endif()
  endif()
  if(NOT APPLE)
    AddHDF5CopyInstallRules(LIBVAR HDF5_LIB
                        LIBNAME hdf5
                        TYPES ${BUILD_TYPES})
    AddHDF5CopyInstallRules(LIBVAR HDF5_CPP_LIB
                        LIBNAME hdf5_cpp
                        TYPES ${BUILD_TYPES})
  endif()
ELSE(HDF5_FOUND)
    MESSAGE(FATAL_ERROR "Cannot build without HDF5.  Please set HDF5_INSTALL environment variable to point to your HDF5 installation.")
ENDif(HDF5_FOUND)

