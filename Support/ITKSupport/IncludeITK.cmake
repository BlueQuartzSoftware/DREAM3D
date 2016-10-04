# -------------------------------------------------------------
# This function adds the necessary cmake code to find the Itk
# shared libraries and setup custom copy commands and/or install
# rules for Linux and Windows to use
function(AddItkCopyInstallRules)
  set(options )
  set(oneValueArgs )
  set(multiValueArgs LIBS TYPES)
  cmake_parse_arguments(itk "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  set(INTER_DIR ".")


  if(MSVC_IDE)
    set(itk_TYPES Debug Release)
  else()
    set(itk_TYPES "${CMAKE_BUILD_TYPE}")
    if("${itk_TYPES}" STREQUAL "")
        set(itk_TYPES "Debug")
    endif()
  endif()

  set(itk_INSTALL_DIR "lib")
  if(WIN32)
    set(itk_INSTALL_DIR ".")
  endif()


  set(STACK "")
  list(APPEND STACK ${itk_LIBS})
  # message(STATUS "STACK: ${STACK}")
  # While the depth-first search stack is not empty
  list(LENGTH STACK STACK_LENGTH)
  #message(STATUS "STACK_LENGTH: ${STACK_LENGTH}")

  while(STACK_LENGTH GREATER 0)

    # This pair of commands "pops" the last value off the
    # stack and sets the STACK_LENGTH variable
    list(GET STACK 0 itk_LIBNAME)
    list(REMOVE_AT STACK 0)
    list(LENGTH STACK STACK_LENGTH)

    # See if we have found a Qt5 or System library. All Itk libs start with "itk"
    string(FIND ${itk_LIBNAME} "Qt5::" IsQt5Lib)
    string(REGEX MATCH "^ITK" IsItkLib ${itk_LIBNAME})

    # If we have not seen this library before then find its dependencies
    if(NOT FOUND_${itk_LIBNAME})
      set(FOUND_${itk_LIBNAME} TRUE PARENT_SCOPE)
      if(${IsQt5Lib} EQUAL -1 AND "${IsItkLib}" STREQUAL "ITK")
        # message(STATUS "    ${itk_LIBNAME}: ${FOUND_${itk_LIBNAME}}  IsItkLib: ${IsItkLib}")
        set(itk_LIBVAR ${itk_LIBNAME})
        foreach(BTYPE ${itk_TYPES} )
          #message(STATUS "  BTYPE: ${BTYPE}")
          string(TOUPPER ${BTYPE} UpperBType)
          if(MSVC_IDE)
            set(INTER_DIR "${BTYPE}")
          endif()

          if(TARGET ${itk_LIBNAME})

            # Find the current library's dependent Itk libraries
            get_target_property(itkLibDeps ${itk_LIBNAME} IMPORTED_LINK_INTERFACE_LIBRARIES_${UpperBType})
            if(NOT "${itkLibDeps}" STREQUAL "itkLibDeps-NOTFOUND" )
              list(APPEND STACK ${itkLibDeps})
            else()
              message(STATUS "---->${itk_LIBNAME} IMPORTED_LINK_INTERFACE_LIBRARIES_${UpperBType} NOT FOUND")
            endif()

            # get_target_property(itkLibDeps ${itk_LIBNAME} INTERFACE_LINK_LIBRARIES)
            # if(NOT "${itkLibDeps}" STREQUAL "itkLibDeps-NOTFOUND" )
            #   list(APPEND STACK ${itkLibDeps})
            # else()
            #   message(STATUS "---->${itk_LIBNAME} INTERFACE_LINK_LIBRARIES NOT FOUND")
            # endif()

            # Get the Actual Library Path and create Install and copy rules
            get_target_property(DllLibPath ${itk_LIBNAME} IMPORTED_LOCATION_${UpperBType})
            if(WIN32)
              string(REGEX MATCH "\.dll$" IsDLL ${DllLibPath})
            else()
              string(REGEX MATCH "\.so$" IsDLL ${DllLibPath})
            endif()
            # message(STATUS "  DllLibPath:(${IsDLL}) ${DllLibPath}")
            if(NOT "${DllLibPath}" STREQUAL "LibPath-NOTFOUND" AND NOT "${IsDLL}" STREQUAL "")
              # message(STATUS "  Creating Install Rule for ${DllLibPath}")
              if(NOT TARGET ZZ_${itk_LIBVAR}_DLL_${UpperBType}-Copy)
                add_custom_target(ZZ_${itk_LIBVAR}_DLL_${UpperBType}-Copy ALL
                                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DllLibPath}
                                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/
                                    # COMMENT "  Copy: ${DllLibPath} To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/"
                                    )
                set_target_properties(ZZ_${itk_LIBVAR}_DLL_${UpperBType}-Copy PROPERTIES FOLDER ZZ_Itk_COPY_FILES)
                install(FILES ${DllLibPath} DESTINATION "${itk_INSTALL_DIR}" CONFIGURATIONS ${BTYPE} COMPONENT Applications)
              endif()
            endif()

            # Now get the path that the library is in
            get_filename_component(${itk_LIBVAR}_DIR ${DllLibPath} PATH)
            # message(STATUS " ${itk_LIBVAR}_DIR: ${${itk_LIBVAR}_DIR}")

            # Now piece together a complete path for the symlink that Linux Needs to have
            if(WIN32)
              get_target_property(${itk_LIBVAR}_${UpperBType} ${itk_LIBNAME} IMPORTED_IMPLIB_${UpperBType})
            else()
              get_target_property(${itk_LIBVAR}_${UpperBType} ${itk_LIBNAME} IMPORTED_SONAME_${UpperBType})
            endif()

            #----------------------------------------------------------------------
            # This section for Linux only
            #message(STATUS "  ${itk_LIBVAR}_${UpperBType}: ${${itk_LIBVAR}_${UpperBType}}")
            if(NOT "${${itk_LIBVAR}_${UpperBType}}" STREQUAL "${itk_LIBVAR}_${UpperBType}-NOTFOUND" AND NOT WIN32)
              set(SYMLINK_PATH "${${itk_LIBVAR}_DIR}/${${itk_LIBVAR}_${UpperBType}}")
              #message(STATUS "  Creating Install Rule for ${SYMLINK_PATH}")
              if(NOT TARGET ZZ_${itk_LIBVAR}_SYMLINK_${UpperBType}-Copy)
                add_custom_target(ZZ_${itk_LIBVAR}_SYMLINK_${UpperBType}-Copy ALL
                                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SYMLINK_PATH}
                                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/
                                    # COMMENT "  Copy: ${SYMLINK_PATH} To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INTER_DIR}/"
                                    )
                set_target_properties(ZZ_${itk_LIBVAR}_SYMLINK_${UpperBType}-Copy PROPERTIES FOLDER ZZ_Itk_COPY_FILES)
                install(FILES ${SYMLINK_PATH} DESTINATION "${itk_INSTALL_DIR}" CONFIGURATIONS ${BTYPE} COMPONENT Applications)
              endif()
            endif()
            # End Linux Only Section
            #------------------------------------------------------------------------
          endif(TARGET ${itk_LIBNAME})
        endforeach()
      endif()
    else()
      # message(STATUS "----> Already Found ${itk_LIBNAME}")
    endif()

    # Remove duplicates and set the stack_length variable (VERY IMPORTANT)
    list(REMOVE_DUPLICATES STACK)
    list(LENGTH STACK STACK_LENGTH)
    #message(STATUS "STACK (${STACK_LENGTH}): ${STACK}")

  endwhile()

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
  AddItkCopyInstallRules(LIBS ${DREAM3D_ITK_MODULES} TYPES ${BUILD_TYPES})
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  GET_FILENAME_COMPONENT (SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
  configure_file("${SELF_DIR}/Deploy_ITK_Libs.sh.in"
                 "${DREAM3DProj_BINARY_DIR}/AdditionalInstallScripts/Deploy_ITK_Libs.sh" @ONLY IMMEDIATE)
endif()

include_directories(${ITK_INCLUDE_DIRS})




