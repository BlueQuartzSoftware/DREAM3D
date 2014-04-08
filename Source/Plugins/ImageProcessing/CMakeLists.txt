#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

project(ImageProcessing)

set(PLUGIN_NAME ImageProcessing)

# --------------------------------------------------------------------
# Look for Qt 4 as we need it for the plugin GUI to be generated
if(NOT QT4_FOUND)
    message(FATAL_ERROR "Qt 4 is Needed for this ${PLUGIN_NAME}Plugin.")
endif()
include( ${QT_USE_FILE} )

# --------------------------------------------------------------------
# find ITK libararies
# list of modules can be found here: http://www.itk.org/Doxygen/html/modules.html
IF(ITK_DIR)
  
  # ITK has been built already
  FIND_PACKAGE(ITK REQUIRED PATHS ${ITK_DIR} NO_DEFAULT_PATH)

  MESSAGE(STATUS "Using ITK available at: ${ITK_DIR}")

  IF("${ITK_LIBRARY_DIRS}" STREQUAL "")
    # ITKv4 does not seem to specify ITK_LIBRARY_DIRS any more, so we generate the binary directory from the main ITK directory
    SET(ITK_LIBRARY_DIRS "${ITK_DIR}/bin")
  ENDIF()

  # Copy libraries to PLUS_EXECUTABLE_OUTPUT_PATH
  IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio" )
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
    FILE(COPY
      ${ITK_LIBRARY_DIRS}/
      DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      FILES_MATCHING REGEX .*${CMAKE_SHARED_LIBRARY_SUFFIX}
      )
  ENDIF()
  SET(DREAM3D_ITK_DIR "${ITK_DIR}" CACHE INTERNAL "Path to store itk binaries")

ELSE(ITK_DIR)
	
	# By default set everything up to download ITK from the DREAM3D web site and we will
	# build inside of our DREAM3D build directory.
	set(ITK_URL "http://dream3d.bluequartz.net/binaries/InsightToolkit-4.5.1.zip")
	set(ITK_PREFIX "${CMAKE_BINARY_DIR}/itk")
	set(ITK_BINARY_DIR "${ITK_PREFIX}/build")
	set(ITK_SOURCE_DIR "${ITK_PREFIX}/src")
	
	# Now look for the DREAM3D_SDK variable that is set on newer versions of DREAM3D. If we
	# find that variable is set to something then we are going to use that directory to 
	# look for the .Zip file containing the source for ITK and then build it.
	if( NOT ${DREAM3D_SDK} STREQUAL "" )
		set(ITK_URL "${DREAM3D_SDK}/InsightToolkit-4.5.1.zip")
		set(ITK_PREFIX ${DREAM3D_SDK}/ITK)
		set(ITK_BINARY_DIR "${DREAM3D_SDK}/ITKBuild")
		set(ITK_SOURCE_DIR "${DREAM3D_SDK}/InsightToolkit-4.5.1")
	endif()

  # We need this for the next command
  include(ExternalProject)
  # ITK has not been built yet, so (possibly) download and build it as an external project
  ExternalProject_Add( itk
    PREFIX ${ITK_PREFIX}
    SOURCE_DIR "${ITK_SOURCE_DIR}"
    BINARY_DIR "${ITK_BINARY_DIR}"
    #--Download step--------------
 #   GIT_REPOSITORY "git://itk.org/ITK.git"
 #   GIT_TAG "421d314ff85ad542ad5c0f3d3c115fa7427b1c64"
    URL ${ITK_URL}
    #--Configure step-------------
    CMAKE_ARGS
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_EXAMPLES:BOOL=OFF
      -DITK_LEGACY_REMOVE:BOOL=ON
      -DKWSYS_USE_MD5:BOOL=ON
      -DITK_USE_REVIEW:BOOL=OFF
      -DITK_BUILD_DEFAULT_MODULES=OFF
      -DITKGroup_Core=ON
      -DITKGroup_Filtering=ON
      -DITKGroup_Registration=ON
      -DITKGroup_Segmentation=ON
    #--Build step-----------------
    #--Install step-----------------
    	INSTALL_COMMAND ""
    	DEPENDS ${ITK_DEPENDENCIES}
    )
	
ENDIF(ITK_DIR)

find_package(ITK COMPONENTS
	#Group Core
	ITKCommon

	#Group IO  ---No HDF5 Modules---
	#ITKIOImageBase

	#Group Filtering
	#ITKDistanceMap
	#ITKThresholding
	#ITKImageGradient
	#ITKImageFilterBase
	#ITKSmoothing
	#ITKDenoising
	#ITKConvolution
	#ITKStatistics
	#ITKImageFeature

	#Group Registration
	ITKRegistrationCommon

	#Group Segmentation
	#ITKVoronoi
	#ITKLevelSets
	ITKWatersheds
	#ITKConnectedComponents

	REQUIRED
)

include(${ITK_USE_FILE})

# --------------------------------------------------------------------
# What image bit depth should be used
# --------------------------------------------------------------------
set(${PLUGIN_NAME}_BitDepth "8" CACHE STRING "Bitdepth for ${PLUGIN_NAME} filters (8, 16, or 32)")
if( ${PLUGIN_NAME}_BitDepth STREQUAL "8" OR ${PLUGIN_NAME}_BitDepth STREQUAL "16" OR ${PLUGIN_NAME}_BitDepth STREQUAL "32")
  add_definitions(-D${PLUGIN_NAME}_BitDepth=${${PLUGIN_NAME}_BitDepth})
else()
  message( WARNING "unsupported ${PLUGIN_NAME}_BitDepth (default bitdepth will be used)" )
endif()


set(${PLUGIN_NAME}_Project_SRCS "")

include(${DREAM3DProj_SOURCE_DIR}/Source/DREAM3DLib/DREAM3DLibMacros.cmake)

#-------
# Start writing this header file
set(AllFiltersHeaderFile ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}Filters/${PLUGIN_NAME}Filters.h_tmp)
file(WRITE ${AllFiltersHeaderFile} "#ifndef _${PLUGIN_NAME}_Filters_H_\n#define _${PLUGIN_NAME}_Filters_H_\n\n")
# --------------------------------------------------------------------
# Create a File that will be used to generate a program that itself generates a
# QFilterWidget derived header for each of the filters based on thier exposed
# filter options
set(CodeGeneratorFile  ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}CodeGenerator.h)
file(WRITE ${CodeGeneratorFile} "")

# --------------------------------------------------------------------
# Create a File that has all the headers for the various Filter Widgets
set(AllFilterWidgetsHeaderFile ${${PLUGIN_NAME}_BINARY_DIR}/FilterWidgetHeaders.h_tmp)
file(WRITE ${AllFilterWidgetsHeaderFile} "#ifndef _ALL${PLUGIN_NAME}_H_\n#define _ALL${PLUGIN_NAME}_H_\n")
# --------------------------------------------------------------------
# Create a File that will register all of the DREAM3D Filters
set(RegisterKnownFilterWidgetsFile ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}Filters/RegisterKnownFilterWidgets.cpp_tmp)
file(WRITE ${RegisterKnownFilterWidgetsFile} "/* This file was AUTO GENERATED During the CMake Run. DO NOT EDIT THIS FILE. Edit the CMake files that generate this file */\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"PipelineBuilder/FilterWidgetManager.h\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"QFilterWidgetFactory.hpp\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"FilterWidgetHeaders.h\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "\nvoid ${PLUGIN_NAME}Plugin::registerFilterWidgets()\n{\n")

# --------------------------------------------------------------------
# Create a File that will register all of the DREAM3D Filters
set(RegisterKnownFiltersFile ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}Filters/RegisterKnownFilters.cpp_tmp)
file(WRITE ${RegisterKnownFiltersFile} "/* This file was AUTO GENERATED During the CMake Run. DO NOT EDIT THIS FILE. Edit the CMake files that generate this file */\n")
file(APPEND ${RegisterKnownFiltersFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")
file(APPEND ${RegisterKnownFiltersFile} "/* This file will be directly included in another source file. */\n")
file(APPEND ${RegisterKnownFiltersFile} "#include \"${PLUGIN_NAME}Filters.h\"\n")
file(APPEND ${RegisterKnownFiltersFile} "\nvoid ${PLUGIN_NAME}Plugin::registerFilters(FilterManager* fm)\n{\n")

# --------------------------------------------------------------------
# Generate a file with all of our MOC'ed sources and  headers
set(FilterWidget_GEN_HDRS_File ${${PLUGIN_NAME}_BINARY_DIR}/FilterWidget_GEN_HDRS_File.cmake)
set(FilterWidget_GEN_SRCS_File ${${PLUGIN_NAME}_BINARY_DIR}/FilterWidget_GEN_SRCS_File.cmake)
file(WRITE ${FilterWidget_GEN_HDRS_File})
file(WRITE ${FilterWidget_GEN_SRCS_File})

set(Project_SRCS "")

# Add in our Filter classes
include(${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Filters/SourceList.cmake)


set(${PLUGIN_NAME}_Project_SRCS ${Project_SRCS})

#-----------------------------------------------------
# Close up all the files that we just generated
file(APPEND ${AllFiltersHeaderFile} "\n#endif\n")
file(APPEND ${AllFilterWidgetsHeaderFile} "\n#endif\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "}\n")
file(APPEND ${RegisterKnownFiltersFile} "\n }\n")

cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}Filters/${PLUGIN_NAME}Filters.h
                          NEW_FILE_PATH  ${AllFiltersHeaderFile} )
cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${${PLUGIN_NAME}_BINARY_DIR}/FilterWidgetHeaders.h
                          NEW_FILE_PATH  ${AllFilterWidgetsHeaderFile} )
cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}Filters/RegisterKnownFilterWidgets.cpp
                          NEW_FILE_PATH  ${RegisterKnownFilterWidgetsFile} )
cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}Filters/RegisterKnownFilters.cpp
                          NEW_FILE_PATH  ${RegisterKnownFiltersFile} )

set(AllFiltersHeaderFile ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}Filters/${PLUGIN_NAME}Filters.h)
set(AllFilterWidgetsHeaderFile ${${PLUGIN_NAME}_BINARY_DIR}/FilterWidgetHeaders.h)
set(RegisterKnownFilterWidgetsFile ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}Filters/RegisterKnownFilterWidgets.cpp)
set(RegisterKnownFiltersFile ${${PLUGIN_NAME}_BINARY_DIR}/${PLUGIN_NAME}Filters/RegisterKnownFilters.cpp)

#--------------------------
include_directories( ${HDF5_INCLUDE_DIR} )
include_directories( ${PROJECT_CODE_DIR} )
include_directories( ${QtSupport_BINARY_DIR} )
include_directories( ${QtSupport_SOURCE_DIR} )
include_directories( ${PipelineBuilderLib_SOURCE_DIR} )
include_directories( ${PipelineBuilderLib_BINARY_DIR} )
include_directories( ${FilterWidgetsLib_SOURCE_DIR} )
include_directories( ${${PLUGIN_NAME}_BINARY_DIR} )
include_directories( ${${PLUGIN_NAME}_SOURCE_DIR}/.. )
include_directories( ${${PLUGIN_NAME}_SOURCE_DIR})

# ******************************************************************************
# Create our custom executable that will generate most of our QFilterWidget
# classes from information stored in the Filters themselves.
# Read the file back into a string so we can configure our file
file(READ ${RegisterKnownFilterWidgetsFile} QFILTERWIDGET_FACTORY_SOURCE)
file(READ ${CodeGeneratorFile} CODE_GEN_FILTER_FUNCTION)
file(READ ${FilterWidget_GEN_HDRS_File} ${PLUGIN_NAME}Widget_GEN_HDRS)
file(READ ${FilterWidget_GEN_SRCS_File} ${PLUGIN_NAME}Widget_GEN_SRCS)

# Remove the temp files that we no longer need
file(REMOVE ${CodeGeneratorFile})
file(REMOVE ${FilterWidget_GEN_HDRS_File})
file(REMOVE ${FilterWidget_GEN_SRCS_File})


set(FILTER_HEADERS "#include \"${AllFiltersHeaderFile}\"")
set(FILTER_WIDGETS_TEMP_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Temp)
set(FILTER_WIDGETS_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR})
set(FILTER_WIDGETS_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR})
set(FILTER_WIDGETS_DOCS_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Documentation/Filters)
set(FILTER_INCLUDE_PREFIX "${PLUGIN_NAME}")
file(MAKE_DIRECTORY ${${PLUGIN_NAME}_BINARY_DIR}/Temp)
configure_file( ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetsCodeGen.h.in
                ${${PLUGIN_NAME}_BINARY_DIR}/FilterWidgetCodeGen.h)

# --------------------------------------------------------------------
# On Linux we need -fPIC for linking static and shared libraries
if(UNIX AND NOT APPLE)
  set_source_files_properties(${${PLUGIN_NAME}_Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC)
  set_source_files_properties(${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetCodeGen.h PROPERTIES COMPILE_FLAGS -fPIC)
  set_source_files_properties(${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetCodeGen.cpp PROPERTIES COMPILE_FLAGS -fPIC)
endif()

add_executable(${PLUGIN_NAME}WidgetCodeGen  ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetCodeGen.cpp
                                            ${${PLUGIN_NAME}_BINARY_DIR}/FilterWidgetCodeGen.h
                                            ${${PLUGIN_NAME}_Project_SRCS}
                                            )
target_link_libraries(${PLUGIN_NAME}WidgetCodeGen MXA EbsdLib DREAM3DLib ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${ITK_LIBRARIES})


# Now run the code to generate the header files which will over write the place
# holder files that were generated from above
add_custom_command(TARGET ${PLUGIN_NAME}WidgetCodeGen POST_BUILD
                    COMMAND $<TARGET_FILE:${PLUGIN_NAME}WidgetCodeGen> )
# ******************************************************************************


add_definitions(-DDREAM3D_USE_QT)
if(MSVC)   # Do not generate manifests for the plugins - caused issues loading plugins
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
endif()


# --------------------------------------------------------------------
# These are the NON-Filter but Qt derived sources of the plugin. In case you have
# additional Qt source files that you need compiled.
set(${PLUGIN_NAME}_HDRS
  ${PROJECT_SOURCE_DIR}/${PLUGIN_NAME}Plugin.h
)

set(${PLUGIN_NAME}_SRCS
 ${PROJECT_SOURCE_DIR}/${PLUGIN_NAME}Plugin.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/" "${${PLUGIN_NAME}_HDRS}" "${${PLUGIN_NAME}_SRCS}" "0")


# --------------------------------------------------------------------
# This section is for a Custom GUI for a Filter. We need to add the Header, Source and UI file here so that
# they do not get auto generated during cmake and build time.
set(${PLUGIN_NAME}_CustomWidgets_HDRS
  # ${PROJECT_SOURCE_DIR}/${PLUGIN_NAME}/${PLUGIN_NAME}FiltersWidgets/Q${PLUGIN_NAME}Widget.h
)

set(${PLUGIN_NAME}_CustomWidgets_SRCS
  # ${PROJECT_SOURCE_DIR}/${PLUGIN_NAME}/${PLUGIN_NAME}FiltersWidgets/Q${PLUGIN_NAME}Widget.cpp
)

QT4_WRAP_UI( ${PLUGIN_NAME}_Generated_UI_HDRS
  # ${PROJECT_SOURCE_DIR}/${PLUGIN_NAME}/${PLUGIN_NAME}FiltersWidgets/Q${PLUGIN_NAME}Widget.ui
)


# --------------------------------------------------------------------
# We need to run Qt's moc system on our Qt based files (Anything deriving from QObject)
# This cmake code sets up moc to be run.
QT4_WRAP_CPP( ${PLUGIN_NAME}_Generated_MOC_SRCS ${${PLUGIN_NAME}Widget_GEN_HDRS} )
QT4_WRAP_CPP( ${PLUGIN_NAME}_MOC_SRCS ${${PLUGIN_NAME}_HDRS} ${${PLUGIN_NAME}_CustomWidgets_HDRS})

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${${PLUGIN_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties( ${${PLUGIN_NAME}_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------------------------
# -- Create a Qt Resource header file of the documentation html files
#QT4_ADD_RESOURCES( ${PLUGIN_NAME}_Generated_RC_SRCS "${HtmlDocQrcFile}"  )

# --------------------------------------------------------------------
# Group all the generated source files in IDE's that support it. Makes it clear
# to the user of the IDE what is generated and what is not.
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "${${PLUGIN_NAME}_GEN_HDRS}" "${${PLUGIN_NAME}_Generated_MOC_SRCS};${${PLUGIN_NAME}_MOC_SRCS};${${PLUGIN_NAME}_GEN_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PLUGIN_NAME}_Generated_UI_HDRS}" "" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${${PLUGIN_NAME}_Generated_RC_SRCS}" "" "0")


# --------------------------------------------------------------------
# Create an install rule for the headers. If you do NOT want your headers
# installed for a deployment to other machines then change the PROJECT_INSTALL_HEADERS
# to a Zero
if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL(FILES ${${PLUGIN_NAME}_HDRS} DESTINATION include/${PLUGIN_NAME}/${PLUGIN_NAME} COMPONENT Headers)
endif()


set(${PLUGIN_NAME}_Project_SRCS
   ${${PLUGIN_NAME}_Project_SRCS}
   ${${PLUGIN_NAME}_HDRS}
   ${${PLUGIN_NAME}_SRCS}
   ${${PLUGIN_NAME}_CustomWidgets_HDRS}
   ${${PLUGIN_NAME}_CustomWidgets_SRCS}
   ${${PLUGIN_NAME}_Generated_UI_HDRS}
   ${${PLUGIN_NAME}Widget_GEN_HDRS}
   ${${PLUGIN_NAME}Widget_GEN_SRCS}
   ${${PLUGIN_NAME}_Generated_MOC_SRCS}
   ${${PLUGIN_NAME}_MOC_SRCS}
   ${${PLUGIN_NAME}_Generated_RC_SRCS}
   )

# --------------------------------------------------------------------
# Setup output Directory for the plugin
if(NOT MSVC)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin/plugins  )
endif()

# --------------------------------------------------------------------
# Add the plugin as a MODULE
add_library(${PLUGIN_NAME} MODULE ${${PLUGIN_NAME}_Project_SRCS})
# --------------------------------------------------------------------
# Link the plugin with other DREAM3D Libraries.
target_link_libraries(${PLUGIN_NAME}
                    MXA
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}
                    ${QT_QTGUI_LIBRARY}
                    QtSupport
                    PipelineBuilderLib
                    ${ITK_LIBRARIES}
                    )

# --------------------------------------------------------------------
# Add a dependency on the code generation step to make sure the code is
# generated before we try to compile the plugin
add_dependencies(${PLUGIN_NAME} ${PLUGIN_NAME}WidgetCodeGen)

# --------------------------------------------------------------------
# Set some additional properties of the plugin like its output name
PluginProperties(${PLUGIN_NAME} "_debug" "${DREAM3D_VERSION}"
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")

# --------------------------------------------------------------------
# Put back the output directory
if(NOT MSVC)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin  )
endif()

