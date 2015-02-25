
 

# -------------------------------------------------------------
# This function adds the necessary cmake code to find the ITK
# shared libraries and setup custom copy commands and/or install
# rules for Linux and Windows to use
function(AddITKCopyInstallRules)
  set(options )
  set(oneValueArgs LIBNAME)
  set(multiValueArgs TYPES)
  cmake_parse_arguments(Z "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  set(INTER_DIR ".")



  set(Z_INSTALL_DIR "lib")
  if(WIN32)
    set(Z_INSTALL_DIR ".")
  endif()

  FOREACH(BTYPE ${Z_TYPES} )
    #message(STATUS "BTYPE: ${BTYPE}")
    STRING(TOUPPER ${BTYPE} TYPE)
    set(INTER_DIR ".")
    if(MSVC_IDE)
      set(INTER_DIR "${BTYPE}")
    endif()

    # Get the Actual Library Path and create Install and copy rules
    set(LibPath "")
    set(LibType "STATIC_LIBRARY")
    if(TARGET ${Z_LIBNAME})
      GET_TARGET_PROPERTY(LibPath ${Z_LIBNAME} IMPORTED_LOCATION_${TYPE})
      GET_TARGET_PROPERTY(LibType ${Z_LIBNAME} TYPE)
    endif()
    if(0)
    message(STATUS "Z_LIBNAME: ${Z_LIBNAME}")
    message(STATUS "Z_TYPES: ${Z_TYPES}")
    message(STATUS "${Z_LIBNAME}: ${LibPath}")
    message(STATUS "BTYPE: ${BTYPE}")
    message(STATUS "LibType: ${LibType}")
    endif()

    # Only do this for the DLL files. Static libraries are not needed
    if(${LibType} STREQUAL "SHARED_LIBRARY")

      install(FILES ${LibPath}
        DESTINATION "${Z_INSTALL_DIR}"
        CONFIGURATIONS ${BTYPE}
        COMPONENT Applications)

      if(NOT TARGET ZZ_${Z_LIBNAME}_DLL_${TYPE}-Copy)
        message(STATUS "Creating Install Rule: ${LibPath}")
        ADD_CUSTOM_TARGET(ZZ_${Z_LIBNAME}_DLL_${TYPE}-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LibPath}
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/
                            COMMENT "  Copy: ${LibPath} To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/")
        set_target_properties(ZZ_${Z_LIBNAME}_DLL_${TYPE}-Copy PROPERTIES FOLDER ZZ_COPY_FILES)
      endif()

    endif()


    if(0)
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
    endif()


  endforeach()

endfunction()



# Make sure the ITK_DIR cmake varaible is set and if it is set then add it to
# the CMAKE_PREFIX_PATH so that it is found
if(EXISTS "${ITK_DIR}")
  set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" ${ITK_DIR})
endif()

# Set the list of ITK Modules that DREAM3D supports using
set(DREAM3D_ITK_MODULES 
   #Group Core
    ITKCommon

    #Group IO  ---No ITK Modules---
    ITKIOImageBase
    ITKIOBMP
    ITKIOJPEG
    ITKIOPNG
    ITKIOTIFF

    #Group Filtering
    ITKImageCompare
    ITKImageIntensity
    #ITKDistanceMap
    #ITKThresholding
    #ITKImageGradient
    #ITKImageFilterBase
    #ITKSmoothing
    #ITKDenoising
    ITKConvolution
    #ITKStatistics
    #ITKImageFeature

    #Group Registration
    ITKRegistrationCommon

    #Group Segmentation
    #ITKVoronoi
    #ITKLevelSets
    ITKWatersheds
    ITKClassifiers
    #ITKConnectedComponents

    #Other
    ITKReview
  )

# --------------------------------------------------------------------
# find ITK libararies
find_package(ITK COMPONENTS ${DREAM3D_ITK_MODULES} REQUIRED)
MESSAGE(STATUS "${PROJECT_NAME}: ITK Location ${ITK_DIR}")
message(STATUS "ITK Version: ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}")

# Include the ITK file 
include(${ITK_USE_FILE})

# Call this macro to make sure ITK actually behaves correctly
itk_module_use(${DREAM3D_ITK_MODULES})

# Start setting up to copy/install the ITK DLL files into the DREAM3D runtime
# directory or package directory. This will probably fail with Xcode
if(MSVC_IDE)
  set(BUILD_TYPES Debug Release)
else()
  set(BUILD_TYPES "${CMAKE_BUILD_TYPE}")
  if("${BUILD_TYPES}" STREQUAL "")
      set(BUILD_TYPES "Debug")
  endif()
endif()

# On Apple Systems since we can use the embedded install_name we do not need 
# any of this copy stuff and the install will take care of itself. This loop
# will iterate over all the ITK Modules, figure out if each is shared and if
# it is shared (DLL), then create a copy rule and an install rule.
if(NOT APPLE)
  foreach(ITKMODULE ${DREAM3D_ITK_MODULES})
    # message(STATUS "=====>${ITKMODULE}")
    AddITKCopyInstallRules(LIBNAME ${ITKMODULE}
                           TYPES ${BUILD_TYPES})
  endforeach()
endif()



# =================== WE SHOULD BE DONE BY THIS POINT =========================

# this section is not needed for Windows or OS X. Still need to figure out for LINUX
if(0)
  IF(ITK_DIR)
    SET(DREAM3D_ITK_DIR "${ITK_DIR}" CACHE INTERNAL "Path to store itk binaries")
    

    # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
    IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
      IF("${ITK_LIBRARY_DIRS}" STREQUAL "")
        # ITKv4 does not seem to specify ITK_LIBRARY_DIRS any more, so we generate the binary directory from the main ITK directory
        SET(ITK_LIBRARY_DIRS "${ITK_DIR}/bin")
      ENDIF()

      FILE(COPY
        ${ITK_LIBRARY_DIRS}/Release/
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
      FILE(COPY
        ${ITK_LIBRARY_DIRS}/Debug/
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
        FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
    ELSE()
      IF("${ITK_LIBRARY_DIRS}" STREQUAL "")
        # ITKv4 does not seem to specify ITK_LIBRARY_DIRS any more, so we generate the binary directory from the main ITK directory
        SET(ITK_LIBRARY_DIRS "${ITK_DIR}/lib/")
      ENDIF()
      #message(STATUS "CMAKE_SHARED_LIBRARY_SUFFIX: ${CMAKE_SHARED_LIBRARY_SUFFIX}")
      #message(STATUS "ITK_LIBRARY_DIRS: ${ITK_LIBRARY_DIRS}")
      FILE(GLOB itkLibs
          "${ITK_LIBRARY_DIRS}*${CMAKE_SHARED_LIBRARY_SUFFIX}.*")
      FILE(COPY
          ${ITK_LIBRARY_DIRS}/
          DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
          FILES_MATCHING PATTERN "*${CMAKE_SHARED_LIBRARY_SUFFIX}.*"
        )

      #message(STATUS "itkLibs: ${itkLibs}")
      install( FILES ${itkLibs}
        DESTINATION "lib"
        COMPONENT Applications
        )
    ENDIF()


  endif()
endif()


