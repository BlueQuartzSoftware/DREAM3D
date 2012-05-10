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
cmake_minimum_required(VERSION 2.8.4)


set(Project_SRCS "")


set(FilterWidget_GEN_SRCS "")
set(FilterWidget_GEN_HDRS "")
# --------------------------------------------------------------------
#
# --------------------------------------------------------------------
Macro(WidgetGeneration DirName)
    set(group ${DirName})
    file(APPEND ${CodeGeneratorFile} "//----- ${DirName} --------------- \n")
    file(APPEND ${HtmlDocQrcFile} "\n")
    # Make sure this directory is created.
    file(MAKE_DIRECTORY ${FilterWidgetsLib_BINARY_DIR}/${DirName}Widgets)
    set(DirName ${DirName})
    include(${PROJECT_CODE_DIR}/DREAM3DLib/${DirName}/SourceList.cmake)
    foreach(file ${${DirName}_FILTERS_HDRS})
        get_filename_component(name ${file} NAME_WE)
        message(STATUS "  ${name}")
        if (NOT EXISTS ${FilterWidgetsLib_SOURCE_DIR}/${DirName}Widgets/Q${name}Widget.h )
            set(FilterWidget_GEN_HDRS ${FilterWidget_GEN_HDRS} ${FilterWidgetsLib_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
            set(FilterWidget_GEN_SRCS ${FilterWidget_GEN_SRCS} ${FilterWidgetsLib_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.cpp)
            if ( NOT EXISTS ${FilterWidgetsLib_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
                set(GENERATED_MOC_SOURCE_FILE "moc_Q${name}Widget.cpp")
                configure_file(${FilterWidgetsLib_SOURCE_DIR}/QFilterWidget_Template.h.in
                              ${FilterWidgetsLib_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
                configure_file(${FilterWidgetsLib_SOURCE_DIR}/QFilterWidget_Template.cpp.in
                              ${FilterWidgetsLib_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.cpp)
            endif()
        endif()
        
        file(APPEND ${HtmlDocQrcFile} "<file>${DirName}/${name}.html</file>\n")
        file(APPEND ${CodeGeneratorFile} "createHeaderFile<${name}>(\"${DirName}\", \"${name}\");\n")
        file(APPEND ${CodeGeneratorFile} "createSourceFile<${name}>(\"${DirName}\", \"${name}\");\n")
        file(APPEND ${CodeGeneratorFile} "createHTMLFile<${name}>(\"${DirName}\", \"${name}\");\n")
    
        file(APPEND ${RegisterKnownFilterWidgetsFile} "   QFilterWidgetFactory<Q${name}Widget>::Pointer q${name}WidgetFactory = QFilterWidgetFactory<Q${name}Widget>::New(); \n")
        file(APPEND ${RegisterKnownFilterWidgetsFile} "   QFilterWidgetManager::Instance()->addFilterWidgetFactory(\"${name}\",q${name}WidgetFactory); \n\n")

        file(APPEND ${AllFilterWidgetsHeaderFile} "#include \"FilterWidgets/${DirName}Widgets/Q${name}Widget.h\"\n")        
    endforeach()
endMacro()

# --------------------------------------------------------------------
# Create a File that will be used to generate a program that itself generates a
# QFilterWidget derived header for each of the filters based on thier exposed
# filter options
set (CodeGeneratorFile  ${FilterWidgetsLib_BINARY_DIR}/CodeGenerator.h)
file(WRITE ${CodeGeneratorFile} "")

# --------------------------------------------------------------------
# We are going to generate this file in the binary directory but not
# really use it currently. If you need to update the FilterDocs.qrc file
# then you can copy this generated file over the file that is in the source
# directory
set (HtmlDocQrcFile ${PipelineBuilderPlugin_BINARY_DIR}/FilterDocs.qrc)
file(WRITE ${HtmlDocQrcFile} "<!DOCTYPE RCC>\n<RCC version=\"1.0\">\n<qresource>\n")


# --------------------------------------------------------------------
# Create a File that has all the headers for the various Filter Widgets
set (AllFilterWidgetsHeaderFile ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetHeaders.h)
file(WRITE ${AllFilterWidgetsHeaderFile} "#ifndef _ALLFilterWidgetsLib_H_\n#define _ALLFilterWidgetsLib_H_\n")

# --------------------------------------------------------------------
# Create a File that will register all of the DREAM3D Filters
set(RegisterKnownFilterWidgetsFile ${FilterWidgetsLib_BINARY_DIR}/RegisterKnownFilterWidgets.cpp)
#file(WRITE ${RegisterKnownFilterWidgetsFile} "#ifndef _RegisterKnownFilterWidgetsFile_H_\n#define _RegisterKnownFilterWidgetsFile_H_\n\n")
file(WRITE ${RegisterKnownFilterWidgetsFile} "#include \"FilterWidgets/QFilterWidgetManager.h\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"FilterWidgets/QFilterWidgetFactory.hpp\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"FilterWidgetHeaders.h\"\n")
#file(APPEND ${RegisterKnownFilterWidgetsFile} "\n\nnamespace DREAM3D { \nnamespace QFilterFactory {\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "\nvoid QFilterWidgetManager::RegisterKnownQFilterWidgets()\n{\n")

set(ONLY_FILTERS 1)
WidgetGeneration(GenericFilters)
WidgetGeneration(IOFilters)
WidgetGeneration(SamplingFilters)
WidgetGeneration(ProcessingFilters)
WidgetGeneration(ReconstructionFilters)
WidgetGeneration(StatisticsFilters)
WidgetGeneration(SurfaceMeshingFilters)
WidgetGeneration(SyntheticBuilderFilters)


file(APPEND ${AllFilterWidgetsHeaderFile} "\n#endif\n")

file(APPEND ${RegisterKnownFilterWidgetsFile} "\n }\n")
#file(APPEND ${RegisterKnownFilterWidgetsFile} "\n}\n}\n #endif")

file(APPEND ${HtmlDocQrcFile} "</qresource>\n</RCC>\n")

# Read the file back into a string so we can configure our file
file(READ ${RegisterKnownFilterWidgetsFile} QFILTERWIDGET_FACTORY_SOURCE)
file(READ ${CodeGeneratorFile} CODE_GEN_FILTER_FUNCTION)
# Remove the temp files that we no longer need
file(REMOVE ${CodeGeneratorFile})

# --------------------------------------------------------------------            
# Create our custom executable that will generate most of our QFilterWidget
# classes from information stored in the Filters themselves. 
set(FilterWidgetsLib_TEMP_DIR ${FilterWidgetsLib_BINARY_DIR}/Temp)
file(MAKE_DIRECTORY ${FilterWidgetsLib_BINARY_DIR}/Temp)
configure_file( ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetsCodeGen.h.in
                ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetCodeGen.h)
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

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT4_WRAP_UI( FilterWidgetsLib_Generated_UI_HDRS 
    ${FilterWidgetsLib_SOURCE_DIR}/ReconstructionFiltersWidgets/QLoadSlicesWidget.ui 
    ${FilterWidgetsLib_SOURCE_DIR}/SyntheticBuilderFiltersWidgets/QInitializeSyntheticVolumeWidget.ui
    ${FilterWidgetsLib_SOURCE_DIR}/IOFiltersWidgets/QEbsdToH5EbsdWidget.ui
    ${FilterWidgetsLib_SOURCE_DIR}/IOFiltersWidgets/QEbsdReferenceFrameDialog.ui
)

# --------------------------------------------------------------------
# Add in ONLY those headers and sources that need to have various Qt related
# utilities run against them
set (QFilterWidget_HDRS
    ${FilterWidgetsLib_SOURCE_DIR}/QFilterWidget.h
    ${FilterWidgetsLib_SOURCE_DIR}/ReconstructionFiltersWidgets/QLoadSlicesWidget.h
    ${FilterWidgetsLib_SOURCE_DIR}/ReconstructionFiltersWidgets/QualityMetricTableModel.h
    ${FilterWidgetsLib_SOURCE_DIR}/ReconstructionFiltersWidgets/QualityMetricItemDelegate.h
    ${FilterWidgetsLib_SOURCE_DIR}/SyntheticBuilderFiltersWidgets/QInitializeSyntheticVolumeWidget.h
    ${FilterWidgetsLib_SOURCE_DIR}/IOFiltersWidgets/QEbsdToH5EbsdWidget.h
    ${FilterWidgetsLib_SOURCE_DIR}/IOFiltersWidgets/QEbsdReferenceFrameDialog.h
)

set(QFilterWidget_SRCS
    ${FilterWidgetsLib_SOURCE_DIR}/QFilterWidget.cpp
    ${FilterWidgetsLib_SOURCE_DIR}/ReconstructionFiltersWidgets/QLoadSlicesWidget.cpp
    ${FilterWidgetsLib_SOURCE_DIR}/ReconstructionFiltersWidgets/QualityMetricTableModel.cpp
    ${FilterWidgetsLib_SOURCE_DIR}/SyntheticBuilderFiltersWidgets/QInitializeSyntheticVolumeWidget.cpp
    ${FilterWidgetsLib_SOURCE_DIR}/IOFiltersWidgets/QEbsdToH5EbsdWidget.cpp
    ${FilterWidgetsLib_SOURCE_DIR}/IOFiltersWidgets/QEbsdReferenceFrameDialog.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets" "${QFilterWidget_HDRS}" "${QFilterWidget_SRCS}" "0")

# --------------------------------------------------------------------
# and finally this will run moc:
QT4_WRAP_CPP( FilterWidgetsLib_Generated_MOC_SRCS ${FilterWidget_GEN_HDRS})
QT4_WRAP_CPP( FilterWidgetsLib_MOC_SRCS ${QFilterWidget_HDRS} )   
# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${FilterWidgetsLib_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties( ${FilterWidgetsLib_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt" "${FilterWidgetsLib_Generated_UI_HDRS}" "${FilterWidgetsLib_Generated_MOC_SRCS};${FilterWidgetsLib_MOC_SRCS}" "0")

# --------------------------------------------------------------------
# These files are generated by our custom command
cmp_IDE_SOURCE_PROPERTIES( "Generated/FilterWidgetCodeGen" "${FilterWidget_GEN_HDRS}" "${FilterWidget_GEN_SRCS}" "0")

# --------------------------------------------------------------------
# Now add in any other headers or sources
set (FilterWidget_HDRS
    ${FilterWidgetsLib_SOURCE_DIR}/IFilterWidgetFactory.h
    ${FilterWidgetsLib_SOURCE_DIR}/QFilterWidgetFactory.hpp
    ${FilterWidgetsLib_SOURCE_DIR}/QFilterWidgetManager.h
)

set(FilterWidget_SRCS
    ${FilterWidgetsLib_SOURCE_DIR}/QFilterWidgetManager.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets" "${FilterWidget_HDRS}" "${FilterWidget_SRCS}" "0")

set (Project_SRCS 
        ${QFilterWidget_HDRS} 
        ${QFilterWidget_SRCS}
        ${FilterWidget_HDRS} 
        ${FilterWidget_SRCS}
        ${FilterWidget_GEN_HDRS}
        ${FilterWidget_GEN_SRCS}
        ${FilterWidgetsLib_MOC_SRCS}
        ${FilterWidgetsLib_Generated_MOC_SRCS}
        ${FilterWidgetsLib_Generated_UI_HDRS}
)



if (UNIX AND NOT APPLE)
    set_source_files_properties(${Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC) 
endif()

add_library(FilterWidgets STATIC ${Project_SRCS})
target_link_libraries(FilterWidgets 
                    MXA
                    H5Support
                    EbsdLib
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY} 
                    ${QT_QTGUI_LIBRARY}
                    QtSupport
                    )
#set_target_properties(FilterWidgets PROPERTIES AUTOMOC TRUE)
#add_dependencies(FilterWidgetsLib_automoc FilterWidgetCodeGen)
add_dependencies(FilterWidgets FilterWidgetCodeGen)
LibraryProperties( FilterWidgets ${EXE_DEBUG_EXTENSION})
