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
PROJECT( EbsdLib )
cmake_minimum_required(VERSION 3.1.0)

if(${CMAKE_MAJOR_VERSION} STREQUAL "3")
	# CMake 3.0 introduces VERSION variables for each Project. We do our own version numbering so we need to have
	# cmake ignore that policy
	cmake_policy(SET CMP0048 OLD)
	# CMake 3.0 introduces some OS X RPath policies
	cmake_policy(SET CMP0042 OLD)
endif()

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
	set(PROJECT_RESOURCES_DIR ${EbsdLib_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
	set(CMP_SOURCE_DIR ${EbsdLib_SOURCE_DIR}/Support/cmp)
endif()

# Bring in our Macros from the CMP project
INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )


set(EbsdLib_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
	set(EbsdLib_BUILT_AS_DYNAMIC_LIB 1)
	if(WIN32)
		ADD_DEFINITIONS("-DEbsdLib_BUILT_AS_DYNAMIC_LIB")
	endif(WIN32)
endif(BUILD_SHARED_LIBS)


# Look for our Prerequisite Libraries
if(NOT DEFINED DREAM3DProj_BINARY_DIR)
	
	# ------------------------------------------------------------------------------
	#  Qt 4 Section
	# ------------------------------------------------------------------------------
	# by default only QtCore and QtGui modules are enabled
	# other modules must be enabled like this:
	if(WIN32)
	   set(QT_USE_QTMAIN TRUE)
	ENDif(WIN32)

	# this command finds Qt4 libraries and sets all required variables
	# note that it's Qt4, not QT4 or qt4
	FIND_PACKAGE( Qt4 REQUIRED )
	if(QT4_FOUND)
	   CMP_COPY_QT4_RUNTIME_LIBRARIES( "QtCore")
	   CMP_QT_LIBRARIES_INSTALL_RULES("QtCore" ".")
	else()
	  message(FATAL_ERROR "Qt version 4.8 Core Library is REQUIRED to build EBSDLib")
	endif()

	set(QT_USE_QTCORE "TRUE")

	# ---------- Find Boost Headers/Libraries -----------------------
	#set(Boost_FIND_REQUIRED FALSE)
	set(Boost_DEBUG FALSE)
	set(Boost_USE_MULTITHREADED TRUE)
	set(Boost_USE_STATIC_LIBS TRUE)
	set(Boost_ADDITIONAL_VERSIONS 1.55.0)
	set(Boost_FIND_COMPONENTS "")
	set(Boost_NO_BOOST_CMAKE 1)
	FIND_PACKAGE(Boost)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
	
	
	# Extra Variables that need to be set before all the configured files are generated.
	option(EbsdLib_ENABLE_HDF5 "Enable HDF5 Support in the EbsdLib" OFF)
	set(EbsdLib_HDF5_SUPPORT "0")

	# ---------- Find HDF5 Headers/Libraries -----------------------
	find_package(HDF5)
	if(HDF5_FOUND)
		file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${HDF5_LIBRARY_DIRS};")
		include_directories(${HDF5_INCLUDE_DIRS} )
		# This copies the DLL (if needed) into the various build directories on MSVC systems
		CMP_COPY_DEPENDENT_LIBRARIES("hdf5")
		# Append the locations of the HDF5 libraries to our Library Search Paths
		list(APPEND CMP_LIB_SEARCH_DIRS ${HDF5_BINARY_DIR} ${HDF5_LIBRARY_DIR} )
		# Create an install rule to copy the dependent shared library over to the installed location
		CMP_LIBRARIES_INSTALL_RULES("hdf5" ".")
	ELSE(HDF5_FOUND)
		MESSAGE(FATAL_ERROR "Cannot build without HDF5.  Please set HDF5_INSTALL environment variable to point to your HDF5 installation.")
	ENDif(HDF5_FOUND)

else()
	set(EbsdLib_HDF5_SUPPORT "1")
	option(EbsdLib_ENABLE_HDF5 "Enable HDF5 Support in the EbsdLib" ON)
endif()

mark_as_advanced(EbsdLib_HDF5_SUPPORT)
mark_as_advanced(EbsdLib_ENABLE_HDF5)


set(PROJECT_PREFIX "Ebsd" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(EbsdLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
if(NOT DEFINED DREAM3DProj_BINARY_DIR)
	set(CMP_HEADER_DIR ${EbsdLib_BINARY_DIR}/EbsdLib)
else()
	set(CMP_HEADER_DIR ${EbsdLib_BINARY_DIR}/EbsdLib)
endif()
set(CMP_CONFIGURATION_FILE_NAME "EbsdLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "EbsdLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "EbsdLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "EbsdLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "EbsdLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(GENERATE_GIT_VERSION 1)
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

if(NOT DEFINED DREAM3DProj_BINARY_DIR)
	configure_file(${EbsdLib_SOURCE_DIR}/EbsdLibConfiguration.h.in
               ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE})
else()
	configure_file(${EbsdLib_SOURCE_DIR}/EbsdLibConfiguration.h.in
               ${EbsdLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})
endif()

# Setup the Include Directories
include_directories(${PROJECT_CODE_DIR})
include_directories(${EbsdLib_BINARY_DIR})
INCLUDE_DIRECTORIES(${EbsdLib_SOURCE_DIR}/..)

#-- Include all the Source Files
include( ${EbsdLib_SOURCE_DIR}/TSL/SourceList.cmake)
include( ${EbsdLib_SOURCE_DIR}/HKL/SourceList.cmake)
include( ${EbsdLib_SOURCE_DIR}/HEDM/SourceList.cmake)



set(EbsdLib_SRCS
    ${EbsdLib_SOURCE_DIR}/AbstractEbsdFields.cpp
    ${EbsdLib_SOURCE_DIR}/EbsdReader.cpp
    ${EbsdLib_SOURCE_DIR}/EbsdTransform.cpp
    )
set(EbsdLib_HDRS
    ${EbsdLib_SOURCE_DIR}/AbstractEbsdFields.h
    ${EbsdLib_SOURCE_DIR}/EbsdReader.h
    ${EbsdLib_SOURCE_DIR}/EbsdTransform.h
    ${EbsdLib_SOURCE_DIR}/EbsdConstants.h
    ${EbsdLib_SOURCE_DIR}/EbsdHeaderEntry.h
    ${EbsdLib_SOURCE_DIR}/EbsdImporter.h
    ${EbsdLib_SOURCE_DIR}/EbsdLibDLLExport.h
    ${EbsdLib_SOURCE_DIR}/EbsdMacros.h
    ${EbsdLib_SOURCE_DIR}/EbsdSetGetMacros.h
)

if(${EbsdLib_ENABLE_HDF5})

	set(EbsdLib_SRCS ${EbsdLib_SRCS}
		${EbsdLib_SOURCE_DIR}/H5EbsdVolumeInfo.cpp
		${EbsdLib_SOURCE_DIR}/H5EbsdVolumeReader.cpp
		)
	set(EbsdLib_HDRS ${EbsdLib_HDRS}
		${EbsdLib_SOURCE_DIR}/H5EbsdVolumeInfo.h
		${EbsdLib_SOURCE_DIR}/H5EbsdVolumeReader.h
	)    
    
endif()


cmp_IDE_SOURCE_PROPERTIES( "EbsdLib/" "${EbsdLib_HDRS}" "${EbsdLib_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(EbsdLib_PROJECT_SRCS
    ${EbsdLib_SRCS}
    ${EbsdLib_HDRS}
    ${TSL_SRCS}
    ${TSL_HDRS}
    ${HKL_SRCS}
    ${HKL_HDRS}
    ${HEDM_SRCS}
    ${HEDM_HDRS}
    ${EbsdLib_Utilities_SRCS}
    ${EbsdLib_Utilities_HDRS}
    "${CMP_HEADER_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}"
)



add_library(${PROJECT_NAME} ${LIB_TYPE} ${EbsdLib_PROJECT_SRCS})

set(EBSDLib_LINK_LIBRARIES "")
if(${EbsdLib_ENABLE_HDF5})
	set(EBSDLib_LINK_LIBRARIES
		${EBSDLib_LINK_LIBRARIES}
		${HDF5_LIBRARIES} H5Support
		)
endif()

if(QT4_FOUND)
	set(EBSDLib_LINK_LIBRARIES
		${EBSDLib_LINK_LIBRARIES}
		 Qt5::Core
		)
endif()

target_link_libraries(${PROJECT_NAME} ${EBSDLib_LINK_LIBRARIES})

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

add_subdirectory(${EbsdLib_SOURCE_DIR}/Test ${EbsdLib_BINARY_DIR}/Test)

