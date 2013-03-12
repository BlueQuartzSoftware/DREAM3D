#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2012, Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#--////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// This code was partly written under US Air Force Contract FA8650-07-D-5800
#///////////////////////////////////////////////////////////////////////////////

project(FilterWidgetsLib)
cmake_minimum_required(VERSION 2.8.9)

set(Project_SRCS "")

# Set up our various files that were written during the DREAM3DLib build phase
set(RegisterKnownFilterWidgetsFile ${FilterWidgetsLib_BINARY_DIR}/RegisterKnownFilterWidgets.cpp)
set(CodeGeneratorFile  ${FilterWidgetsLib_BINARY_DIR}/CodeGenerator.h)
set(FilterWidget_GEN_HDRS_File ${FilterWidgetsLib_BINARY_DIR}/FilterWidget_GEN_HDRS_File.cmake)
set(FilterWidget_GEN_SRCS_File ${FilterWidgetsLib_BINARY_DIR}/FilterWidget_GEN_SRCS.cmake)

# Read the file back into a string so we can configure our file
file(READ ${RegisterKnownFilterWidgetsFile} QFILTERWIDGET_FACTORY_SOURCE)
file(READ ${CodeGeneratorFile} CODE_GEN_FILTER_FUNCTION)
file(READ ${FilterWidget_GEN_HDRS_File} FilterWidget_GEN_HDRS)
file(READ ${FilterWidget_GEN_SRCS_File} FilterWidget_GEN_SRCS)

# Remove the temp files that we no longer need
file(REMOVE ${CodeGeneratorFile})
#file(REMOVE ${FilterWidget_GEN_HDRS_File})
#file(REMOVE ${FilterWidget_GEN_SRCS_File})

# --------------------------------------------------------------------
# Create our custom executable that will generate most of our QFilterWidget
# classes from information stored in the Filters themselves.
set(FILTER_HEADERS "#include \"DREAM3DLib/DREAM3DFilters.h\"")
set(FILTER_WIDGETS_TEMP_DIR ${FilterWidgetsLib_BINARY_DIR}/Temp)
set(FILTER_WIDGETS_BINARY_DIR ${FilterWidgetsLib_BINARY_DIR}/DREAM3DLib)
set(FILTER_WIDGETS_SOURCE_DIR ${FilterWidgetsLib_SOURCE_DIR})
set(FILTER_WIDGETS_DOCS_DIR ${DREAM3DProj_SOURCE_DIR}/Documentation/Filters)
set(FILTER_INCLUDE_PREFIX "DREAM3DLib")

file(MAKE_DIRECTORY ${FilterWidgetsLib_BINARY_DIR}/Temp)
configure_file( ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetsCodeGen.h.in
                ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetCodeGen.h)
if(MSVC)
  set_source_files_properties(${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetCodeGen.h PROPERTIES COMPILE_FLAGS /bigobj)
  set_source_files_properties(${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetCodeGen.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()
add_executable(FilterWidgetCodeGen  ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetCodeGen.cpp
                                    ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetCodeGen.h)
target_link_libraries(FilterWidgetCodeGen MXA EbsdLib DREAM3DLib)


# Now run the code to generate the header files which will over write the place
# holder files that were generated from above
add_custom_command(TARGET FilterWidgetCodeGen POST_BUILD
                    COMMAND $<TARGET_FILE:FilterWidgetCodeGen> )


# --------------------------------------------------------------------
# Set some Include directories
include_directories(${FilterWidgetsLib_SOURCE_DIR}/..)
include_directories(${FilterWidgetsLib_SOURCE_DIR})
include_directories(${FilterWidgetsLib_BINARY_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/PipelineBuilder)
include_directories(${DREAM3DProj_BINARY_DIR}/PipelineBuilder)
include_directories(${DREAM3DProj_BINARY_DIR}/QtSupport)

# --------------------------------------------------------------------
# Add in ONLY those headers and sources that need to have various Qt related
# utilities run against them
set(IOFILTER_WIDGET_HDRS
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QReadH5EbsdWidget.h
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QEbsdToH5EbsdWidget.h
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QEbsdReferenceFrameDialog.h
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QDataContainerReaderWidget.h
)
set(IOFILTER_WIDGET_SRCS
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QReadH5EbsdWidget.cpp
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QEbsdToH5EbsdWidget.cpp
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QEbsdReferenceFrameDialog.cpp
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QDataContainerReaderWidget.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets/IOFiltersWidgets" "${IOFILTER_WIDGET_HDRS}" "${IOFILTER_WIDGET_SRCS}" "0")
set(SYNTHETIC_WIDGET_HDRS
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/SyntheticBuildingFiltersWidgets/QInitializeSyntheticVolumeWidget.h
)
set(SYNTHETIC_WIDGET_SRCS
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/SyntheticBuildingFiltersWidgets/QInitializeSyntheticVolumeWidget.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets/SyntheticBuildingFiltersWidgets" "${SYNTHETIC_WIDGET_HDRS}" "${SYNTHETIC_WIDGET_SRCS}" "0")
set(SAMPLING_WIDGET_HDRS
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/SamplingFiltersWidgets/QHex2SqrConverterWidget.h
)
set(SAMPLING_WIDGET_SRCS
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/SamplingFiltersWidgets/QHex2SqrConverterWidget.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets/SamplingFiltersWidgets" "${SAMPLING_WIDGET_HDRS}" "${SAMPLING_WIDGET_SRCS}" "0")

set(STATISTICS_WIDGET_HDRS
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/StatisticsFiltersWidgets/QGenerateEnsembleStatisticsWidget.h
)
set(STATISTICS_WIDGET_SRCS
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/StatisticsFiltersWidgets/QGenerateEnsembleStatisticsWidget.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets/StatisticsFiltersWidgets" "${STATISTICS_WIDGET_HDRS}" "${STATISTICS_WIDGET_SRCS}" "0")

set(QFilterWidget_HDRS
    ${IOFILTER_WIDGET_HDRS} ${SYNTHETIC_WIDGET_HDRS} ${SAMPLING_WIDGET_HDRS} ${STATISTICS_WIDGET_HDRS}
)

set(QFilterWidget_SRCS
    ${IOFILTER_WIDGET_SRCS} ${SYNTHETIC_WIDGET_SRCS} ${SAMPLING_WIDGET_SRCS} ${STATISTICS_WIDGET_SRCS}
)


# --------------------------------------------------------------------
# and finally this will run moc:
QT4_WRAP_CPP( FilterWidgetsLib_Generated_MOC_SRCS ${FilterWidget_GEN_HDRS})
QT4_WRAP_CPP( FilterWidgetsLib_MOC_SRCS ${QFilterWidget_HDRS} )
# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${FilterWidgetsLib_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties( ${FilterWidgetsLib_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# -- Run MOC and UIC on the necessary files
# QT4_ADD_RESOURCES( FilterWidgetsLib_Generated_RC_SRCS "${DREAM3DProj_SOURCE_DIR}/Documentation/Filters/Generated_FilterDocs.qrc"  )

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT4_WRAP_UI( FilterWidgetsLib_Generated_UI_HDRS
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/SyntheticBuildingFiltersWidgets/QInitializeSyntheticVolumeWidget.ui
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/SamplingFiltersWidgets/QHex2SqrConverterWidget.ui
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QReadH5EbsdWidget.ui
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QEbsdToH5EbsdWidget.ui
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QEbsdReferenceFrameDialog.ui
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/IOFiltersWidgets/QDataContainerReaderWidget.ui
    ${FilterWidgetsLib_SOURCE_DIR}/DREAM3DLib/StatisticsFiltersWidgets/QGenerateEnsembleStatisticsWidget.ui
)

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${FilterWidgetsLib_Generated_MOC_SRCS};${FilterWidgetsLib_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${FilterWidgetsLib_Generated_UI_HDRS}" "" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${FilterWidgetsLib_Generated_RC_SRCS}" "" "0")

# --------------------------------------------------------------------
# These files are generated by our custom command
cmp_IDE_SOURCE_PROPERTIES( "Generated/FilterWidgets" "${FilterWidget_GEN_HDRS}" "${FilterWidget_GEN_SRCS}" "0")

# --------------------------------------------------------------------
# Now add in any other headers or sources
set(FilterWidget_HDRS
    ${FilterWidgetsLib_SOURCE_DIR}/QFilterWidgetFactory.hpp
    ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetsLib.h
)

set(FilterWidget_SRCS
    ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetsLib.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets" "${FilterWidget_HDRS}" "${FilterWidget_SRCS}" "0")

set(Project_SRCS
        ${QFilterWidget_HDRS}
        ${QFilterWidget_SRCS}
        ${FilterWidget_HDRS}
        ${FilterWidget_SRCS}
        ${FilterWidget_GEN_HDRS}
        ${FilterWidget_GEN_SRCS}
        ${FilterWidgetsLib_MOC_SRCS}
        ${FilterWidgetsLib_Generated_MOC_SRCS}
        ${FilterWidgetsLib_Generated_UI_HDRS}
        ${FilterWidgetsLib_Generated_RC_SRCS}
)



if(UNIX AND NOT APPLE)
    set_source_files_properties(${Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

add_library(FilterWidgetsLib STATIC ${Project_SRCS})
target_link_libraries(FilterWidgetsLib
                    MXA
                    H5Support
                    EbsdLib
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}
                    ${QT_QTGUI_LIBRARY}
                    QtSupport
                    PipelineBuilderLib
                    )
#set_target_properties(FilterWidgets PROPERTIES AUTOMOC TRUE)
#add_dependencies(FilterWidgetsLib_automoc FilterWidgetCodeGen)
add_dependencies(FilterWidgetsLib FilterWidgetCodeGen)
LibraryProperties( FilterWidgetsLib ${EXE_DEBUG_EXTENSION})
