#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, 2010, 2011 Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, 2010, 2011 Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#--////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// This code was partly written under US Air Force Contract FA8650-07-D-5800
#///////////////////////////////////////////////////////////////////////////////
# set project's name
PROJECT(OrientationLib VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR})

if(NOT DEFINED DREAM3DProj_BINARY_DIR)

  # ---------- Setup output Directories -------------------------
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Libraries"
    )

  # --------- Setup the Executable output Directory -------------
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Executables."
    )

  # --------- Setup the Executable output Directory -------------
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all static libraries."
    )

  set(EXE_DEBUG_EXTENSION "_debug")
  set(PROJECT_INSTALL_HEADERS "0")
  set(PROJECT_INSTALL_EXECUTABLES "1")

endif()

if(NOT PROJECT_RESOURCES_DIR)
  set(PROJECT_RESOURCES_DIR ${OrientationLib_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
  set(CMP_SOURCE_DIR ${OrientationLib_SOURCE_DIR}/Support/cmp)
endif()

# Bring in our Macros from the CMP project
INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )


set(OrientationLib_BUILT_AS_DYNAMIC_LIB 0)
if(BUILD_SHARED_LIBS)
  set(OrientationLib_BUILT_AS_DYNAMIC_LIB 1)
  if(WIN32)
    ADD_DEFINITIONS("-DOrientationLib_BUILT_AS_DYNAMIC_LIB")
  endif(WIN32)
endif(BUILD_SHARED_LIBS)


# Look for our Prerequisite Libraries
if(NOT DEFINED DREAM3DProj_BINARY_DIR)
  message(FATAL_ERROR "This library is included in DREAM3D. If you are trying to build this outside of DREAM3D there are prerequisites that you need to complete first.")

else()
  set(OrientationLib_HDF5_SUPPORT "1")
  option(OrientationLib_ENABLE_HDF5 "Enable HDF5 Support in the OrientationLib" ON)
endif()

mark_as_advanced(OrientationLib_HDF5_SUPPORT)
mark_as_advanced(OrientationLib_ENABLE_HDF5)


set(PROJECT_PREFIX "OrientationLib" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(OrientationLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
if(NOT DEFINED DREAM3DProj_BINARY_DIR)
  set(CMP_HEADER_DIR ${OrientationLib_BINARY_DIR}/OrientationLib)
else()
  set(CMP_HEADER_DIR ${OrientationLib_BINARY_DIR}/OrientationLib)
endif()
set(CMP_CONFIGURATION_FILE_NAME "OrientationLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "OrientationLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "OrientationLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "OrientationLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "OrientationLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(GENERATE_GIT_VERSION 1)
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

if(NOT DEFINED DREAM3DProj_BINARY_DIR)
  cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${OrientationLib_SOURCE_DIR}/OrientationLibConfiguration.h.in
                            GENERATED_FILE_PATH ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE})
else()
  cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${OrientationLib_SOURCE_DIR}/OrientationLibConfiguration.h.in
                            GENERATED_FILE_PATH ${OrientationLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})
endif()

cmp_IDE_GENERATED_PROPERTIES("Generated"
                             "${OrientationLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE};${OrientationLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_CONFIGURATION_FILE_NAME};${OrientationLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_TYPES_FILE_NAME};${OrientationLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_VERSION_HEADER_FILE_NAME}"
                             "${OrientationLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_VERSION_SOURCE_FILE_NAME}")

# Setup the Include Directories
include_directories(${PROJECT_CODE_DIR})
include_directories(${OrientationLib_BINARY_DIR})
include_directories(${OrientationLib_SOURCE_DIR}/..)

#-- Include all the Source Files
include( ${OrientationLib_SOURCE_DIR}/SpaceGroupOps/SourceList.cmake)
include( ${OrientationLib_SOURCE_DIR}/OrientationMath/SourceList.cmake)
include( ${OrientationLib_SOURCE_DIR}/IO/SourceList.cmake)
include( ${OrientationLib_SOURCE_DIR}/Texture/SourceList.cmake)
include( ${OrientationLib_SOURCE_DIR}/Utilities/SourceList.cmake)

set(OrientationLib_SRCS

)
set(OrientationLib_HDRS
    ${OrientationLib_SOURCE_DIR}/OrientationLibDLLExport.h
)

if(${OrientationLib_ENABLE_HDF5})

  set(OrientationLib_SRCS ${OrientationLib_SRCS}
  )
  set(OrientationLib_HDRS ${OrientationLib_HDRS}
  )

endif()


cmp_IDE_SOURCE_PROPERTIES( "OrientationLib/" "${OrientationLib_HDRS}" "${OrientationLib_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(OrientationLib_PROJECT_SRCS
    ${OrientationLib_SRCS}
    ${OrientationLib_HDRS}
    ${OrientationLib_SpaceGroupOps_HDRS}
    ${OrientationLib_SpaceGroupOps_SRCS}
    ${OrientationLib_OrientationMath_HDRS}
    ${OrientationLib_OrientationMath_SRCS}
    ${OrientationLib_IO_HDRS}
    ${OrientationLib_IO_SRCS}
    ${OrientationLib_Utilities_HDRS}
    ${OrientationLib_Utilities_SRCS}
    ${OrientationLib_Texture_HDRS}
    ${OrientationLib_Texture_SRCS}
    #-- Our Generated Header Files
    ${OrientationLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE}
    ${OrientationLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_CONFIGURATION_FILE_NAME}
    ${OrientationLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_TYPES_FILE_NAME}
    ${OrientationLib_BINARY_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
    ${OrientationLib_BINARY_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)



add_library(${PROJECT_NAME} ${LIB_TYPE} ${OrientationLib_PROJECT_SRCS})

set(OrientationLib_LINK_LIBRARIES "")
if(${OrientationLib_ENABLE_HDF5})
  set(OrientationLib_LINK_LIBRARIES
    ${OrientationLib_LINK_LIBRARIES}
    hdf5 H5Support
    )
endif()

if(Qt5_FOUND)
  set(OrientationLib_LINK_LIBRARIES
     ${OrientationLib_LINK_LIBRARIES}
     Qt5::Core
    )
endif()

 # On mac systems, we likely have to link against the vecLib framework
if(APPLE)
  find_library(LAPACK_LIB Accelerate)
  include_directories("-FvecLib")
endif(APPLE)

target_link_libraries(${PROJECT_NAME} SIMPLib ${OrientationLib_LINK_LIBRARIES} ${LAPACK_LIB})

LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} )

set(install_dir "tools")
set(lib_install_dir "lib")
if(WIN32)
  set(install_dir ".")
  set(lib_install_dir "")
endif()


if(BUILD_SHARED_LIBS AND NOT APPLE)
  INSTALL(TARGETS ${PROJECT_NAME}
    COMPONENT Applications
    RUNTIME DESTINATION ${install_dir}
    LIBRARY DESTINATION ${lib_install_dir}
    ARCHIVE DESTINATION ${lib_install_dir}
    BUNDLE DESTINATION "."
  )
endif()

add_subdirectory(${OrientationLib_SOURCE_DIR}/Test ${OrientationLib_BINARY_DIR}/Test)

