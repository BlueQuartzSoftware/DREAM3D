


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

      GET_TARGET_PROPERTY(LibPath ${Z_LIBNAME} LOCATION_${TYPE})
      GET_TARGET_PROPERTY(LibType ${Z_LIBNAME} TYPE)

      if(0)
        message(STATUS "********************************************")
        message(STATUS "Z_LIBNAME: ${Z_LIBNAME}")
        message(STATUS "Z_TYPES: ${Z_TYPES}")
        message(STATUS "LibPath: ${LibPath}")
        message(STATUS "LibType: ${LibType}")
        message(STATUS "BTYPE: ${BTYPE}")
        message(STATUS "${Z_LIBNAME}_LIBRARIES: ${${Z_LIBNAME}_LIBRARIES}s")
      endif()

      # Only do this for the DLL files. Static libraries are not needed
      if(${LibType} STREQUAL "SHARED_LIBRARY")
        # Create an install rule for the library if we are NOT on Linux
        if(NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
          install(FILES ${LibPath}
                  DESTINATION "${Z_INSTALL_DIR}"
                  CONFIGURATIONS ${BTYPE}
                  COMPONENT Applications)
        endif()
        GET_FILENAME_COMPONENT(ITK_LIBRARY_DIR ${LibPath} PATH)
        set(ITK_LIBRARY_DIR ${ITK_LIBRARY_DIR} CACHE  STRING "" FORCE)
        # Create a rule to copy the dylib/DLL into the BUILD/Bin directory so the executables will run
        # but only if the specific copy rule has NOT already been generated
        if(NOT TARGET ZZ_${Z_LIBNAME}_DLL_${TYPE}-Copy)
          #message(STATUS "Creating Copy Rule Lib:${Z_LIBNAME} Location:${LibPath}")
          ADD_CUSTOM_TARGET(ZZ_${Z_LIBNAME}_DLL_${TYPE}-Copy ALL
                              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LibPath}
                              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/
                          #    COMMENT "Copy: ${LibPath} To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/"
                            )
          set_target_properties(ZZ_${Z_LIBNAME}_DLL_${TYPE}-Copy PROPERTIES FOLDER ZZ_COPY_FILES)
        endif()

      endif()

    endif()

  endforeach()

endfunction()



# Make sure the ITK_DIR cmake varaible is set and if it is set then add it to
# the CMAKE_PREFIX_PATH so that it is found
if(EXISTS "${ITK_DIR}")
  set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" ${ITK_DIR})
endif()

set(ITK_LIBRARY_DIR "" CACHE  STRING "" FORCE)

# Set the list of ITK Modules that DREAM3D supports using
set(DREAM3D_CORE_ITK_MODULES
    #Group Core
    ITKCommon

    #Group Filtering
    ITKImageCompare
    ITKImageIntensity
    #ITKDistanceMap
    #ITKThresholding
    #ITKImageGradient
    #ITKImageFilterBase
    #ITKSmoothing
    ITKDenoising
    ITKConvolution
    ITKStatistics
    #ITKImageFeature

    #Group Registration
    ITKRegistrationCommon

    #Group Segmentation
    #ITKVoronoi
    #ITKLevelSets
    ITKWatersheds
    ITKClassifiers
    #ITKConnectedComponents
    ITKTransform

    #Other
    ITKReview
  )

get_property(DREAM3D_ADDITIONAL_ITK_MODULES GLOBAL PROPERTY DREAM3D_ADDITIONAL_ITK_MODULES)
list(APPEND DREAM3D_ITK_MODULES
    ${DREAM3D_CORE_ITK_MODULES}
    ${DREAM3D_ADDITIONAL_ITK_MODULES}
  )

# --------------------------------------------------------------------
# find ITK libararies
# Does not register IO factories. It will be done by each plugin that needs it.
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER TRUE)
find_package(ITK COMPONENTS ${DREAM3D_ITK_MODULES} REQUIRED)

get_property(Itk_STATUS_PRINTED GLOBAL PROPERTY Itk_STATUS_PRINTED)
if(NOT Itk_STATUS_PRINTED)
  message(STATUS "${PROJECT_NAME}: ITK Location ${ITK_DIR} ITK Version: ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}")
  set_property(GLOBAL PROPERTY Itk_STATUS_PRINTED TRUE)

  # Now append ITK_RUNTIME_LIBRARY_DIRS path to the SIMPLibSearchDirs property
  get_property(SIMPLibSearchDirs GLOBAL PROPERTY SIMPLibSearchDirs)
  if(NOT "${SIMPLibSearchDirs}" STREQUAL "")
    file(APPEND "${SIMPLibSearchDirs}" "${ITK_RUNTIME_LIBRARY_DIRS};")
  endif()

endif()


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
# will iterate over all the ITK Modules, figure out if each is shared
# (DLL), then create a copy rule and an install rule.
if(NOT APPLE)
  foreach(ITKMODULE ${DREAM3D_ITK_MODULES})
    AddITKCopyInstallRules(LIBNAME ${ITKMODULE}
                           TYPES ${BUILD_TYPES})
  endforeach()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  GET_FILENAME_COMPONENT (SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
  configure_file("${SELF_DIR}/Deploy_ITK_Libs.sh.in"
                 "${DREAM3DProj_BINARY_DIR}/AdditionalInstallScripts/Deploy_ITK_Libs.sh" @ONLY IMMEDIATE)
endif()

include_directories(${ITK_INCLUDE_DIRS})




