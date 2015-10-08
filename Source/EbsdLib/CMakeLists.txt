# ============================================================================
# Copyright (c) 2009-2015 BlueQuartz Software, LLC
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
# contributors may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The code contained herein was partially funded by the followig contracts:
#    United States Air Force Prime Contract FA8650-07-D-5800
#    United States Air Force Prime Contract FA8650-10-D-5210
#    United States Prime Contract Navy N00173-07-C-2068
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
# set project's name
PROJECT( EbsdLib VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR})

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
	
	
	# ---------- Find and Use the Qt5 Libraries -----------------------
	include(${CMP_SOURCE_DIR}/ExtLib/Qt5Support.cmake)

	# Extra Variables that need to be set before all the configured files are generated.
	option(EbsdLib_ENABLE_HDF5 "Enable HDF5 Support in the EbsdLib" OFF)
	set(EbsdLib_HDF5_SUPPORT "0")
	# ---------- Find HDF5 Headers/Libraries -----------------------
	# HDF5 now comes with everything that is needed for CMake to load
	# up the targets (Exported) that it needs. We just need to find where
	# HDF5 is installed.
	include(${CMP_SOURCE_DIR}/ExtLib/HDF5Support.cmake)

	# ---------- Find Boost Headers/Libraries -----------------------
	#set(Boost_FIND_REQUIRED FALSE)
	set(Boost_DEBUG FALSE)
	set(Boost_USE_MULTITHREADED TRUE)
	set(Boost_USE_STATIC_LIBS TRUE)
	set(Boost_ADDITIONAL_VERSIONS 1.57)
	set(Boost_FIND_COMPONENTS "")
	set(Boost_NO_BOOST_CMAKE 1)
	FIND_PACKAGE(Boost)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
	

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
	set(CMP_HEADER_DIR ${EbsdLib_BINARY_DIR})
else()
	set(CMP_HEADER_DIR ${EbsdLib_BINARY_DIR})
endif()
set(CMP_CONFIGURATION_FILE_NAME "EbsdLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "EbsdLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "EbsdLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "EbsdLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "EbsdLib.h")
set(CMP_PROJECT_NAMESPACE "EbsdLib")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(GENERATE_GIT_VERSION 1)
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

configure_file(${EbsdLib_SOURCE_DIR}/EbsdLibConfiguration.h.in
               ${EbsdLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

# Setup the Include Directories
include_directories(${PROJECT_CODE_DIR})
include_directories(${EbsdLib_BINARY_DIR})
INCLUDE_DIRECTORIES(${EbsdLib_SOURCE_DIR}/..)

#-- Include all the Source Files
include( ${EbsdLib_SOURCE_DIR}/TSL/SourceList.cmake)
include( ${EbsdLib_SOURCE_DIR}/HKL/SourceList.cmake)

set(EbsdLib_Generated_HDRS
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_CONFIGURATION_FILE_NAME}
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_TYPES_FILE_NAME}
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_TOP_HEADER_FILE}
  )
set(EbsdLib_Generated_SRCS
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
  )
cmp_IDE_GENERATED_PROPERTIES("Generated" "${EbsdLib_Generated_HDRS}" "${EbsdLib_Generated_SRCS}")

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


cmp_IDE_SOURCE_PROPERTIES( "" "${EbsdLib_HDRS}" "${EbsdLib_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(EbsdLib_PROJECT_SRCS
    ${EbsdLib_SRCS}
    ${EbsdLib_HDRS}
    ${TSL_SRCS}
    ${TSL_HDRS}
    ${HKL_SRCS}
    ${HKL_HDRS}
    ${EbsdLib_Utilities_SRCS}
    ${EbsdLib_Utilities_HDRS}
    "${EbsdLib_Generated_SRCS}"
    "${EbsdLib_Generated_HDRS}"
)



add_library(${PROJECT_NAME} ${LIB_TYPE} ${EbsdLib_PROJECT_SRCS})

set(EBSDLib_LINK_LIBRARIES "")
if(${EbsdLib_ENABLE_HDF5})
	set(EBSDLib_LINK_LIBRARIES
		${EBSDLib_LINK_LIBRARIES}
		hdf5 H5Support
		)
endif()

if(QT5_FOUND)
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

include(${EbsdLib_SOURCE_DIR}/Test/CMakeLists.txt)

