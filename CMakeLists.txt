
if(${CMAKE_MAJOR_VERSION} VERSION_GREATER_EQUAL "3.1.0")
  cmake_policy(SET CMP0002 NEW)
  cmake_policy(SET CMP0054 NEW)
endif()

if(${CMAKE_MAJOR_VERSION} VERSION_GREATER_EQUAL "3.12.0")
  cmake_policy(SET CMP0074 NEW)
endif()

if(${CMAKE_MAJOR_VERSION} VERSION_GREATER_EQUAL "3.13.0")
  cmake_policy(SET CMP0079 NEW)
endif()

cmake_minimum_required(VERSION 3.10.0)

message(STATUS "********* STARTING DREAM.3D CONFIGURATION ***********************")
if(NOT "${DREAM3D_SDK}" STREQUAL "")
  include("${DREAM3D_SDK}/DREAM3D_SDK.cmake")
  get_property(DREAM3D_SDK_STATUS_PRINTED GLOBAL PROPERTY DREAM3D_SDK_STATUS_PRINTED)
  if(NOT DREAM3D_SDK_STATUS_PRINTED)
    #message(STATUS "DREAM3D_SDK Location: ${DREAM3D_SDK}")
    set_property(GLOBAL PROPERTY DREAM3D_SDK_STATUS_PRINTED TRUE)
  endif()
else()
  message(STATUS "You have elected to NOT set a DREAM3D_SDK CMake variable. You will")
  message(STATUS "need to point to the various dependecies that DREAM.3D requires for")
  message(STATUS "building. Those that are undefined are listed next:")

  if(NOT DEFINED HDF5_DIR)
      message(STATUS "Set HDF5_DIR variable to the directory where the hdf5-config.cmake is located.")
  endif()

  if(NOT DEFINED ITK_DIR)
      message(STATUS "Set ITK_DIR variable to the directory where the ITKConfig.cmake is located.")
  endif()

  if(NOT DEFINED EIGEN_INSTALL)
      message(STATUS "Set EIGEN_INSTALL variable to the root directory of the Eigen installation.")
  endif()

  if(NOT DEFINED Qt5_DIR)
      message(STATUS "Set Qt5_DIR variable to the directory where the Qt5Config.cmake is located.")
  endif()

  if(NOT DEFINED QWT_INSTALL)
      message(STATUS "Set QWT_INSTALL variable to the root directory where Qwt is located.")
  endif()

  if(NOT DEFINED TBB_DIR)
      message(STATUS "Set TBB_DIR variable to the root directory where TBB is located.")
  endif()

endif()

# Request C++11 standard, using new CMake variables.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)


#------------------------------------------------------------------------------
# Create the Top level Project
project(DREAM3DProj VERSION 6.5.0.0)
set(PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)

# ---------- Setup output Directories -------------------------
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${DREAM3DProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Libraries"
    )
endif()

# --------- Setup the Executable output Directory -------------
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${DREAM3DProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Executables."
    )
endif()

# --------- Setup the Executable output Directory -------------
if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${DREAM3DProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all static libraries."
    )
endif()


#------------------------------------------------------------------------------
# Find the Git Package for Versioning. It should be ok if Git is NOT found
Find_package(Git)

get_filename_component(DREAM3D_PARENT ${DREAM3DProj_SOURCE_DIR} DIRECTORY CACHE)
set(DREAM3D_ExternalProject_DIR "${DREAM3DProj_SOURCE_DIR}/ExternalProjects")
set(UnitTestAnalysisFile ${DREAM3DProj_BINARY_DIR}/UnitTestAnalysis.md)
file(WRITE ${UnitTestAnalysisFile} "## Unit Test Analysis ##\n")
set(TotalFilters 0)
set(TotalTestedFilters 0)

#------------------------------------------------------------------------------
# If the developer has set another CMP directory then use that, otherwise look
# for the CMP directory at the same level as the SIMPL directory
if("${CMP_SOURCE_DIR}" STREQUAL "")
  if(EXISTS "${DREAM3DProj_SOURCE_DIR}/ExternalProjects/CMP")
    set(CMP_SOURCE_DIR "${DREAM3DProj_SOURCE_DIR}/ExternalProjects/CMP")
  elseif(EXISTS "${DREAM3D_PARENT}/CMP")
    set(CMP_SOURCE_DIR ${DREAM3D_PARENT}/CMP)
  else()
    message(FATAL_ERROR "CMP_SOURCE_DIR was not set. Where is the CMP project directory. Please set the CMP_SOURCE_DIR variable to the CMP directory.")
  endif()
  message(STATUS "CMP_SOURCE_DIR: ${CMP_SOURCE_DIR}")
endif()

#------------------------------------------------------------------------------
# If the developer has set another SIMPL directory then use that, otherwise look
# for the SIMPLView directory at the same level as the SIMPL directory
if("${SIMPLProj_SOURCE_DIR}" STREQUAL "")
  if(EXISTS "${DREAM3DProj_SOURCE_DIR}/ExternalProjects/SIMPL")
    set(SIMPLProj_SOURCE_DIR "${DREAM3DProj_SOURCE_DIR}/ExternalProjects/SIMPL")
  elseif(EXISTS "${DREAM3D_PARENT}/SIMPL")
    set(SIMPLProj_SOURCE_DIR ${DREAM3D_PARENT}/SIMPL)
  else()
    message(FATAL_ERROR "SIMPLProj_SOURCE_DIR was not set. Where is the CMP project directory. Please set the SIMPLProj_SOURCE_DIR variable to the CMP directory.")
  endif()
  message(STATUS "SIMPLProj_SOURCE_DIR: ${SIMPLProj_SOURCE_DIR}")
endif()


#------------------------------------------------------------------------------
# If the developer has set another SIMPLView directory then use that, otherwise look
# for the SIMPLView directory at the same level as the SIMPL directory
if("${SIMPLViewProj_SOURCE_DIR}" STREQUAL "")
  if(EXISTS "${DREAM3DProj_SOURCE_DIR}/ExternalProjects/SIMPLView")
    set(SIMPLViewProj_SOURCE_DIR "${DREAM3DProj_SOURCE_DIR}/ExternalProjects/SIMPLView")
  elseif(EXISTS "${DREAM3D_PARENT}/SIMPLView")
    set(SIMPLViewProj_SOURCE_DIR "${DREAM3D_PARENT}/SIMPLView")
  else()
    message(FATAL_ERROR "SIMPLViewProj_SOURCE_DIR was not set. Where is the CMP project directory. Please set the SIMPLViewProj_SOURCE_DIR variable to the CMP directory.")
  endif()
  message(STATUS "SIMPLViewProj_SOURCE_DIR: ${SIMPLViewProj_SOURCE_DIR}")
endif()

set_property(GLOBAL PROPERTY COPY_LIBRARY_TARGETS "")

#------------------------------------------------------------------------------
# There are several different Release Types that we are going to use.
# Official: This is an official release from BlueQuartz Software
# Beta: This is a Beta release that has is getting ready to be officially released.
# Development: This is currently in development and probably has bugs.
get_property(DREAM3DProj_RELEASE_TYPE GLOBAL PROPERTY PACKAGE_RELEASE_TYPE)
if("${DREAM3DProj_RELEASE_TYPE}" STREQUAL "")
  set(DREAM3DProj_RELEASE_TYPE "Development")
  set_property(GLOBAL PROPERTY PACKAGE_RELEASE_TYPE "${DREAM3DProj_RELEASE_TYPE}")
endif()

include(CMakeParseArguments)

#------------------------------------------------------------------------------
# If we have not defined a "BrandedSIMPLView_DIR" then define it to the default
# generic set of ICONs. Needs to be defined BEFORE we setup the SIMPLView project
if(NOT DEFINED BrandedSIMPLView_DIR)
  set(BrandedSIMPLView_DIR ${DREAM3DProj_SOURCE_DIR}/Resources/OpenSourceEdition)
endif()

# Set the Application Name to DREAM3D to rebrand SIMPLView with the correct icons and badges
set(SIMPLView_APPLICATION_NAME "DREAM3D")
set(SIMPLView_VERSION_HDR_FILE "SIMPLViewVersion.h")
set(SIMPLView_VERSION_SRC_FILE "SIMPLViewVersion.cpp")
set(EXE_DEBUG_EXTENSION "_debug")

# -----------------------------------------------------------------------
# Set these CMP variables because the Version Files need them set
set(CMP_TOP_HEADER_FILE "SIMPLView.h")
set(CMP_CONFIGURED_FILES_SOURCE_DIR ${CMP_SOURCE_DIR}/ConfiguredFiles)


INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )

# -----------------------------------------------------------------------
# Generate a set of Version Cpp/H files that will be used by SIMPLView to
# get and display the proper version numbers.
cmpRevisionString( GENERATED_HEADER_FILE_PATH "${SIMPLView_VERSION_HDR_FILE}"
                        GENERATED_SOURCE_FILE_PATH "${SIMPLView_VERSION_SRC_FILE}"
                        NAMESPACE "SIMPLView"
                        PROJECT_NAME "${PROJECT_NAME}"
                        EXPORT_MACRO "SIMPLView_EXPORT")

set_property(GLOBAL PROPERTY SIMPLViewProj_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set_property(GLOBAL PROPERTY SIMPLViewProj_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set_property(GLOBAL PROPERTY SIMPLViewProj_VERSION_PATCH "${${PROJECT_NAME}_VERSION_PATCH}")
set_property(GLOBAL PROPERTY SIMPLViewProj_VERSION_TWEAK "${${PROJECT_NAME}_VERSION_TWEAK}")

make_directory("${DREAM3DProj_BINARY_DIR}/SIMPLView/SIMPLView")
file(RENAME "${DREAM3DProj_BINARY_DIR}/${SIMPLView_VERSION_HDR_FILE}" "${DREAM3DProj_BINARY_DIR}/SIMPLView/SIMPLView/${SIMPLView_VERSION_HDR_FILE}")
file(RENAME "${DREAM3DProj_BINARY_DIR}/${SIMPLView_VERSION_SRC_FILE}" "${DREAM3DProj_BINARY_DIR}/SIMPLView/SIMPLView/${SIMPLView_VERSION_SRC_FILE}")

set(SIMPLView_VERSION_HDR_FILE "${DREAM3DProj_BINARY_DIR}/SIMPLView/SIMPLView/${SIMPLView_VERSION_HDR_FILE}")
set(SIMPLView_VERSION_SRC_FILE "${DREAM3DProj_BINARY_DIR}/SIMPLView/SIMPLView/${SIMPLView_VERSION_SRC_FILE}")

# -----------------------------------------------------------------------
# Setup a Global property that is used to gather Documentation Information
# into a single known location
get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
if("${DREAM3DDocRoot}" STREQUAL "")
  set_property(GLOBAL PROPERTY DREAM3DDocRoot "${DREAM3DProj_BINARY_DIR}/Documentation")
  get_property(DREAM3DDocRoot GLOBAL PROPERTY DREAM3DDocRoot)
endif()


get_property(SIMPLPluginList GLOBAL PROPERTY SIMPLPluginList)
if("${SIMPLPluginList}" STREQUAL "")
  set_property(GLOBAL PROPERTY SIMPLPluginList "${DREAM3DProj_BINARY_DIR}/SIMPLPluginList.txt")
  get_property(SIMPLPluginList GLOBAL PROPERTY SIMPLPluginList)
  file(WRITE "${SIMPLPluginList}" "")
endif()


get_property(SIMPLibSearchDirs GLOBAL PROPERTY SIMPLibSearchDirs)
if("${SIMPLibSearchDirs}" STREQUAL "")
  set_property(GLOBAL PROPERTY SIMPLibSearchDirs "${DREAM3DProj_BINARY_DIR}/LibSearchDirs.txt")
  get_property(SIMPLibSearchDirs GLOBAL PROPERTY SIMPLibSearchDirs)
  file(WRITE "${SIMPLibSearchDirs}" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY};")
endif()

# -----------------------------------------------------------------------
# Set a global value to NOT install headers
# -----------------------------------------------------------------------
set_property(GLOBAL PROPERTY CMP_PROJECT_INSTALL_HEADERS "0")

# -----------------------------------------------------------------------
# Set a Global Prefix where everthing is going to get intalled. For OS X we are
# going to try to install everything into the .app package
# -----------------------------------------------------------------------
set_property(GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX ".")
if(APPLE)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX "${SIMPLView_APPLICATION_NAME}${EXE_DEBUG_EXTENSION}.app/Contents/")
  else()
    set_property(GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX "${SIMPLView_APPLICATION_NAME}.app/Contents/")
  endif()
endif()
get_property(DREAM3D_PACKAGE_DEST_PREFIX GLOBAL PROPERTY DREAM3D_PACKAGE_DEST_PREFIX)


# ------- Enable the CTest testing. ---------------
OPTION(SIMPL_BUILD_TESTING "Compile the test programs" ON)
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
    #ENABLE_TESTING()
    include( CTest )
endif()



# --------------------------------------------------------------------
# This is set to "1" so that our test files are deleted at the end of the tests
set(DREAM3D_REMOVE_TEST_FILES "1")

# --------------------------------------------------------------------
# Over ride CMake's built in module directory by prepending cmp's module
# directory first
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMP_MODULES_SOURCE_DIR} )

# Build shared libraries
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" ON)
set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  #message(FATAL_ERROR "Due to issues exporting symbols with MSVC DREAM3D does NOT currently support intermediate shared libraries. Please turn OFF BUILD_SHARED_LIBS")
    set(LIB_TYPE SHARED)
    list(APPEND CMP_LIB_SEARCH_DIRS  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    if(MSVC)
        list(APPEND CMP_LIB_SEARCH_DIRS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
                                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release )
    endif()

else()
  message(FATAL_ERROR "BUILD_SHARED_LIBS Must be set to ON in order to build properly.")
endif(BUILD_SHARED_LIBS)


# -----------------------------------------------------------------------
# Bring in the SIMPLView Project, which will bring in SIMPLib
# -----------------------------------------------------------------------
add_subdirectory( ${SIMPLViewProj_SOURCE_DIR} ${PROJECT_BINARY_DIR}/SIMPLView)

# --------------------------------------------------------------------
# Find and Use the Qt5 Libraries
include(${CMP_SOURCE_DIR}/ExtLib/Qt5Support.cmake)
set(DREAM3D_Qt5_Components Core Widgets Network Gui Concurrent Xml OpenGL PrintSupport Sql)
CMP_AddQt5Support( "${DREAM3D_Qt5_Components}"
                    "${SIMPL_USE_QtWebEngine}"
                    "${DREAM3DProj_BINARY_DIR}"
                    "DREAM3D")

#Enable use of Solution Folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PROJECT_RESOURCES_DIR ${DREAM3DProj_SOURCE_DIR}/Resources)
set(PROJECT_INSTALL_HEADERS "0")
set(PROJECT_INSTALL_EXECUTABLES "1")


set(DREAM3D_DOCUMENTATION_DIR ${PROJECT_SOURCE_DIR}/Documentation)
set(DREAM3D_SUPPORT_DIR ${PROJECT_SOURCE_DIR}/Support)


set(CMP_ENABLE_PLUGINS "1")
set(CMP_PLUGIN_LIST_FILE "${SIMPLPluginList}")
set(CMP_PLUGIN_SEARCHDIR_FILE "${SIMPLibSearchDirs}")
file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin/plugins;")
file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin;")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_MODULES_SOURCE_DIR ${CMP_SOURCE_DIR}/Modules CACHE INTERNAL "")

if(UNIX AND NOT APPLE)
    set(DREAM3D_LINUX_SYSTEM ${CMAKE_SYSTEM_NAME} CACHE STRING "The Name of the linux distrubution for packaging")
endif()


# --------------------------------------------------------------------
#
if("${DREAM3D_DATA_DIR}" STREQUAL "")
  set(DREAM3D_DATA_DIR "${DREAM3D_PARENT}/DREAM3D_Data/"  CACHE PATH "The directory where to find test data files")
endif()

if("${DREAM3D_DATA_DIR}" STREQUAL "")
   message(FATAL_ERROR "The DREAM3D_DATA_DIR variable is not set and it needs to be set to a location where the test data is located")
endif()


# --------------------------------------------------------------------
# Now that we have found all of our required libraries and packages we can start the all
# the tests

# We do NOT want to install any of the headers
set(DREAM3DProj_INSTALL_FILES "${PROJECT_INSTALL_HEADERS}")

# --------------------------------------------------------------------
# Generate our ReadMe and License Files
configure_file(${PROJECT_RESOURCES_DIR}/DREAM3D/DREAM3DLicense.txt
                ${PROJECT_BINARY_DIR}/DREAM3D/DREAM3DLicense.txt )

# --------------------------------------------------------------------
# Generate install rules for the text files
if(APPLE)
  set(INSTALL_DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}/Resources")
else()
  set(INSTALL_DESTINATION "${DREAM3D_PACKAGE_DEST_PREFIX}")
endif()
install(FILES ${PROJECT_BINARY_DIR}/DREAM3D/DREAM3DLicense.txt
        DESTINATION "${INSTALL_DESTINATION}"
        COMPONENT Applications)



# Include some directories for the compiler

# -----------------------------------------------------------------------
# -- Build the EbsdLib Library
#add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/libharu ${PROJECT_BINARY_DIR}/libharu)

# -----------------------------------------------------------------------
# -- Build the EbsdLib Library
add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/EbsdLib ${PROJECT_BINARY_DIR}/EbsdLib)

# -----------------------------------------------------------------------
# -- Build the OrientationLib Library
add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/OrientationLib ${PROJECT_BINARY_DIR}/OrientationLib)

# -----------------------------------------------------------------------
# This needs to be set here as we are going to look for files in this directory
# -----------------------------------------------------------------------
set(FilterWidgetsLib_BINARY_DIR ${DREAM3DProj_BINARY_DIR}/FilterWidgetsLib)

#- Add in the Main DREAM.3D Application
set(DREAM3D_DOCS_ROOT_DIR "${DREAM3DProj_BINARY_DIR}/Bin/Help/DREAM3D")

#-------------------------------------------------------------------------------
# Compile the Core Plugins that come with DREAM3D and any other Plugins that the
# developer has added.
include(${PROJECT_CODE_DIR}/Plugins/Plugins.cmake)

# add the Command line utilities
# Figure out here if we are going to build the command line tools
option(SIMPL_BUILD_CLI_TOOLS "Build the command line versions of the applications" ON)
if( SIMPL_BUILD_CLI_TOOLS )
  add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Tools ${PROJECT_BINARY_DIR}/Tools)

  if(TARGET PipelineRunner AND APPLE)
    set(QAB_BINARY_DIR ${DREAM3DProj_BINARY_DIR}/Tools)
    set(QAB_TARGET "PipelineRunner")
    set(QAB_COMPONENT Applications)
    set(osx_app_name "PipelineRunner")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      set(osx_app_name "PipelineRunner_debug")
    endif()

    set(OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT
                        "${QAB_BINARY_DIR}/OSX_Scripts/${QAB_TARGET}_CompleteTool.cmake")
    set(OPTIMIZE_BUNDLE_SHELL_SCRIPT
                        "${QAB_BINARY_DIR}/OSX_Scripts/${QAB_TARGET}_OptimizeTool.sh")

    set(PROJECT_INSTALL_DIR "bin")
    configure_file( "${CMP_OSX_TOOLS_SOURCE_DIR}/CompleteTool.cmake.in"
                    "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)

    configure_file("${CMP_OSX_TOOLS_SOURCE_DIR}/CompleteTool.sh.in"
                    "${OPTIMIZE_BUNDLE_SHELL_SCRIPT}" @ONLY IMMEDIATE)

    install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" COMPONENT ${QAB_COMPONENT})

  endif()
endif()

#-------------------------------------------------------------------------------
# Should we build the documentation
get_property(SIMPLView_BUILD_DOCUMENTATION GLOBAL PROPERTY SIMPLView_BUILD_DOCUMENTATION)
message(STATUS "SIMPLView_BUILD_DOCUMENTATION: ${SIMPLView_BUILD_DOCUMENTATION}")
if(SIMPLView_BUILD_DOCUMENTATION)
  set(DOCUMENTATION_RESOURCES_DIR ${DREAM3DProj_SOURCE_DIR}/Resources/DocConfigFiles)

  # Now add each subdirectory that holds our documentation files
  add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Documentation/ReferenceManual ${PROJECT_BINARY_DIR}/Documentation/ReferenceManual)
endif()

# This should be the last line in this file:
include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

# ------- Enable the CTest testing. Use make test to run all tests ---------------
# -------------------------------------------------------------------- 
# If Testing is enabled, turn on the Unit Tests 
if(SIMPL_BUILD_TESTING)
    include(${DREAM3DProj_SOURCE_DIR}/Test/CMakeLists.txt)
endif()

#-------------------------------------------------------------------------------
# Include code to copy the prebuilt pipelines, data and other items
include(${DREAM3DProj_SOURCE_DIR}/Support/Support.cmake)

#------------------------------------------------------------------------------------
# After we are all done, then update the LibrarySearchDirs.txt file
file(APPEND "${SIMPLibSearchDirs}" "${CMP_LIB_SEARCH_DIRS};")

#------------------------------------------------------------------------------------
# Print out a Summary Section:

message(STATUS "*******************************************************************************")
message(STATUS "* DREAM.3D Configuration Summary")
message(STATUS "* BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "* System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "* Version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "* Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "* DREAM3_SDK: ${DREAM3D_SDK}")
message(STATUS "* DREAM3D_DATA_DIR: ${DREAM3D_DATA_DIR}")
message(STATUS "* CMP_SOURCE_DIR: ${CMP_SOURCE_DIR}")
message(STATUS "* SIMPLProj_SOURCE_DIR: ${SIMPLProj_SOURCE_DIR}")
message(STATUS "* SIMPLViewProj_SOURCE_DIR: ${SIMPLViewProj_SOURCE_DIR}")
message(STATUS "* BrandedSIMPLView_DIR: ${BrandedSIMPLView_DIR}")
message(STATUS "* Eigen (${EIGEN_VERSION}) ${EIGEN_INSTALL}")
message(STATUS "* HDF5 (${HDF5_VERSION}) ${HDF5_INSTALL}")
message(STATUS "* ITK (${ITK_VERSION}) ${ITK_DIR}")
message(STATUS "* Qt5 (${Qt5_VERSION}) ${Qt5_DIR}")
message(STATUS "* Qwt (${QWT_VERSION}) ${QWT_INSTALL}")
message(STATUS "* TBB (${TBB_VERSION}) ${TBB_INSTALL_DIR}")
message(STATUS "* -------------- Plugins ------------------------------------------------------")
foreach(d3dPlugin ${DREAM3D_ALL_PLUGINS})
  get_property(PluginNumFilters GLOBAL PROPERTY ${d3dPlugin}_NumFilters)
  message(STATUS "*   ${d3dPlugin}: [${DREAM3D_BUILD_PLUGIN_${d3dPlugin}}] ${PluginNumFilters} Filters")
endforeach()
message(STATUS "*******************************************************************************")

# get_target_property( sources dream3d_py SOURCES)
# foreach(source ${sources})
#   message(STATUS "${source}")  
# endforeach(source ${sources})

