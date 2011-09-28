
if (NOT DEFINED CMP_SOURCE_DIR)
get_filename_component(CMP_SOURCE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH CACHE)
endif()
set(CMP_CONFIGURED_FILES_SOURCE_DIR ${CMP_SOURCE_DIR}/ConfiguredFiles CACHE INTERNAL "")
set(CMP_CORE_TESTS_SOURCE_DIR ${CMP_SOURCE_DIR}/CoreTests CACHE INTERNAL "")
set(CMP_INSTALLATION_SUPPORT_SOURCE_DIR ${CMP_SOURCE_DIR}/InstallationSupport CACHE INTERNAL "")
set(CMP_MODULES_SOURCE_DIR ${CMP_SOURCE_DIR}/Modules CACHE INTERNAL "")
set(CMP_OSX_TOOLS_SOURCE_DIR ${CMP_SOURCE_DIR}/OSX_Tools CACHE INTERNAL "")

# --------------------------------------------------------------------
# Over ride CMake's built in module directory by prepending cmp's module
# directory first
SET (CMAKE_MODULE_PATH ${CMP_MODULES_SOURCE_DIR} ${CMAKE_MODULE_PATH})

INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )

include ( ${CMP_CORE_TESTS_SOURCE_DIR}/cmpConfigureChecks.cmake )
if (NOT DEFINED CMP_PROJECT_NAMESPACE)
    set(CMP_PROJECT_NAMESPACE "CMP")
endif()

if (NOT DEFINED CMP_HEADER_DIR)
    set (CMP_HEADER_DIR ${PROJECT_BINARY_DIR}/cmp)
endif()

if (NOT DEFINED CMP_CONFIGURATION_FILE_NAME)
    set (CMP_CONFIGURATION_FILE_NAME "cmpConfiguration.h")
endif()

if (NOT DEFINED CMP_TYPES_FILE_NAME)
    set (CMP_TYPES_FILE_NAME "cmpTypes.h")
endif()

if (NOT DEFINED CMP_VERSION_HEADER_FILE_NAME)
    set (CMP_VERSION_HEADER_FILE_NAME "cmpVersion.h")
endif()

if (NOT DEFINED CMP_TOP_HEADER_FILE)
    set (CMP_TOP_HEADER_FILE "")
endif()

if (NOT DEFINED CMP_PROJECT_NAME)
    set (CMP_PROJECT_NAME "CMP")
endif()

if (NOT DEFINED CMP_PLUGIN_LIST_FILE)
    set (CMP_PLUGIN_LIST_FILE ${PROJECT_BINARY_DIR}/plugins.txt)
endif()

if (NOT DEFINED CMP_PLUGIN_SEARCHDIR_FILE)
    set (CMP_PLUGIN_SEARCHDIR_FILE ${PROJECT_BINARY_DIR}/libsearchdirs.txt)
endif()


#message(STATUS "CMP_HEADER_DIR: ${CMP_HEADER_DIR}")
#message(STATUS "CMP_CONFIGURATION_FILE_NAME: ${CMP_CONFIGURATION_FILE_NAME}")
#message(STATUS "CMP_TYPES_FILE_NAME: ${CMP_TYPES_FILE_NAME}")

get_filename_component(CMP_CONFIGURATION_HEADER_GUARD ${CMP_CONFIGURATION_FILE_NAME} NAME_WE)
get_filename_component(CMP_TYPES_HEADER_GUARD ${CMP_TYPES_FILE_NAME} NAME_WE)
get_filename_component(CMP_VERSION_HEADER_GUARD ${CMP_VERSION_HEADER_FILE_NAME} NAME_WE)

# --------------------------------------------------------------------
# Generate our header files
# --------------------------------------------------------------------
configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpConfiguration.h.in
               ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME} )
configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpPrimitiveTypes.h.in
               ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME} )


# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
if( ${CMP_GENERATE_VERSION_STRING} )
    cmpVersionStringsFromGit( GENERATED_FILE_PATH "${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}" 
                          NAMESPACE "${CMP_PROJECT_NAMESPACE}" 
                          cmpProjectName "${CMP_PROJECT_NAME}")
endif()

cmp_IDE_GENERATED_PROPERTIES( "Generated" 
"${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}" 
"${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}" 
"${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}")

# --------------------------------------------------------------------
# Enable the use of plugins that will get generated as part of the project
# We are going to write the paths to the plugins into a file and then that
# file will be used as input to set an actual cmake variable and then 
# passed to the bundle utilities cmake macro.
if (CMP_ENABLE_PLUGINS)

file(WRITE ${CMP_PLUGIN_LIST_FILE} "")
file(WRITE ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin/plugins;")
file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin;")

endif()


   