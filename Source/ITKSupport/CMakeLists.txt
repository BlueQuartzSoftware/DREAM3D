if(true)


## YOU MUST INCLUDE THIS FILE TO USE ITK !!!
## Then do not forget to add "${ITK_LIBRARIES}" to the target_link_libraries below
include( ${DREAM3D_SUPPORT_DIR}/ITKSupport/IncludeITK.cmake)
include_directories(${ITK_INCLUDE_DIRS})




function(sitk_install_exported_target tgt)
  # Remove the build tree's SimpleITKTargets file if this is the first call:
  get_property(first_time GLOBAL PROPERTY SimpleITK_FIRST_EXPORTED_TARGET)
  if(NOT first_time)
    file(REMOVE ${CMAKE_BINARY_DIR}/SimpleITKTargets.cmake)
    set_property(GLOBAL PROPERTY SimpleITK_FIRST_EXPORTED_TARGET 1)
  endif()

  set_property(TARGET ${tgt} PROPERTY VERSION 1)
  set_property(TARGET ${tgt} PROPERTY SOVERSION 1)
  set_property(TARGET ${tgt} PROPERTY
    OUTPUT_NAME ${tgt}-${SimpleITK_VERSION_MAJOR}.${SimpleITK_VERSION_MINOR})

  export(TARGETS ${tgt}
    APPEND FILE "${CMAKE_BINARY_DIR}/SimpleITKTargets.cmake"
    )

  install(TARGETS ${tgt}
    EXPORT SimpleITKTargets
    RUNTIME DESTINATION ${SimpleITK_INSTALL_RUNTIME_DIR}
    LIBRARY DESTINATION ${SimpleITK_INSTALL_LIBRARY_DIR}
    ARCHIVE DESTINATION ${SimpleITK_INSTALL_ARCHIVE_DIR}
    )
endfunction()

set(SimpleITK_INSTALL_RUNTIME_DIR "Bin")
set(SimpleITK_INSTALL_LIBRARY_DIR "Bin")
set(SimpleITK_INSTALL_ARCHIVE_DIR "Bin")

PROJECT( SimpleITKExplicit VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR})


include_directories( ${SimpleITKExplicit_SOURCE_DIR}/Explicit/include)
include( Explicit/src/CMakeLists.txt)


else()
PROJECT( ITKSupport VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR})

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
  set(PROJECT_RESOURCES_DIR ${ITKSupport_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
  set(CMP_SOURCE_DIR ${ITKSupport_SOURCE_DIR}/Support/cmp)
endif()

# Bring in our Macros from the CMP project
INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )


set(ITKSupport_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
  set(ITKSupport_BUILT_AS_DYNAMIC_LIB 1)
  if(WIN32)
    ADD_DEFINITIONS("-DITKSupport_BUILT_AS_DYNAMIC_LIB")
  endif(WIN32)
endif(BUILD_SHARED_LIBS)


# Look for our Prerequisite Libraries
if(NOT DEFINED DREAM3DProj_BINARY_DIR)
  
  
  # ---------- Find and Use the Qt5 Libraries -----------------------
  include(${CMP_SOURCE_DIR}/ExtLib/Qt5Support.cmake)

  # Extra Variables that need to be set before all the configured files are generated.
  option(ITKSupport_ENABLE_HDF5 "Enable HDF5 Support in the ITKSupport" OFF)
  set(ITKSupport_HDF5_SUPPORT "0")
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
  set(ITKSupport_HDF5_SUPPORT "1")
  option(ITKSupport_ENABLE_HDF5 "Enable HDF5 Support in the ITKSupport" ON)
endif()

mark_as_advanced(ITKSupport_HDF5_SUPPORT)
mark_as_advanced(ITKSupport_ENABLE_HDF5)


set(PROJECT_PREFIX "Ebsd" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(ITKSupport_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
if(NOT DEFINED DREAM3DProj_BINARY_DIR)
  set(CMP_HEADER_DIR ${ITKSupport_BINARY_DIR})
else()
  set(CMP_HEADER_DIR ${ITKSupport_BINARY_DIR})
endif()
set(CMP_CONFIGURATION_FILE_NAME "ITKSupportConfiguration.h")
set(CMP_TYPES_FILE_NAME "ITKSupportTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "ITKSupportVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "ITKSupportVersion.cpp")
set(CMP_TOP_HEADER_FILE "ITKSupport.h")
set(CMP_PROJECT_NAMESPACE "ITKSupport")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(GENERATE_GIT_VERSION 1)
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

configure_file(${ITKSupport_SOURCE_DIR}/ITKSupportConfiguration.h.in
               ${ITKSupport_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

# Setup the Include Directories
include_directories(${PROJECT_CODE_DIR})
include_directories(${ITKSupport_BINARY_DIR})
INCLUDE_DIRECTORIES(${ITKSupport_SOURCE_DIR}/..)



set(ITKSupport_Generated_HDRS
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_CONFIGURATION_FILE_NAME}
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_TYPES_FILE_NAME}
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_TOP_HEADER_FILE}
  )
set(ITKSupport_Generated_SRCS
  ${${PROJECT_NAME}_BINARY_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
  )
cmp_IDE_GENERATED_PROPERTIES("Generated" "${ITKSupport_Generated_HDRS}" "${ITKSupport_Generated_SRCS}")

set(ITKSupport_SRCS
    #${ITKSupport_SOURCE_DIR}/
    )
set(ITKSupport_HDRS
    #${ITKSupport_SOURCE_DIR}/
)




cmp_IDE_SOURCE_PROPERTIES( "" "${ITKSupport_HDRS}" "${ITKSupport_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(ITKSupport_PROJECT_SRCS
    ${ITKSupport_SRCS}
    ${ITKSupport_HDRS}
    "${ITKSupport_Generated_SRCS}"
    "${ITKSupport_Generated_HDRS}"
)



add_library(${PROJECT_NAME} ${LIB_TYPE} ${ITKSupport_PROJECT_SRCS})

set(ITKSupport_LINK_LIBRARIES "")
if(${ITKSupport_ENABLE_HDF5})
  set(ITKSupport_LINK_LIBRARIES
    ${ITKSupport_LINK_LIBRARIES}
    hdf5 H5Support
    )
endif()

if(QT5_FOUND)
  set(ITKSupport_LINK_LIBRARIES
    ${ITKSupport_LINK_LIBRARIES}
     Qt5::Core
    )
endif()


## YOU MUST INCLUDE THIS FILE TO USE ITK !!!
## Then do not forget to add "${ITK_LIBRARIES}" to the target_link_libraries below
include( ${DREAM3D_SUPPORT_DIR}/ITKSupport/IncludeITK.cmake)
include_directories(${ITK_INCLUDE_DIRS})


target_link_libraries(${PROJECT_NAME} ${ITKSupport_LINK_LIBRARIES} ${ITK_LIBRARIES})

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

endif()
