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

project(FilterWidgets)
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
    # Make sure this directory is created.
    file(MAKE_DIRECTORY ${FilterWidgets_BINARY_DIR}/${DirName}Widgets)
    
    include(${PROJECT_CODE_DIR}/DREAM3DLib/${DirName}/SourceList.cmake)
    foreach(file ${${DirName}_FILTERS_HDRS})
        get_filename_component(name ${file} NAME_WE)
        message(STATUS "  ${name}")
        if (NOT EXISTS ${FilterWidgets_SOURCE_DIR}/${DirName}Widgets/Q${name}Widget.h )
            set(FilterWidget_GEN_HDRS ${FilterWidget_GEN_HDRS} ${FilterWidgets_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
            set(FilterWidget_GEN_SRCS ${FilterWidget_GEN_SRCS} ${FilterWidgets_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.cpp)
            if ( NOT EXISTS ${FilterWidgets_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
                configure_file(${FilterWidgets_SOURCE_DIR}/QFilterWidget.h.in
                              ${FilterWidgets_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
                configure_file(${FilterWidgets_SOURCE_DIR}/QFilterWidget.h.in
                              ${FilterWidgets_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.cpp)
            endif()
        endif()
          
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
set (CodeGeneratorFile  ${FilterWidgets_BINARY_DIR}/CodeGenerator.h)
file(WRITE ${CodeGeneratorFile} "")

# --------------------------------------------------------------------
# Create a File that has all the headers for the various Filter Widgets
set (AllFilterWidgetsHeaderFile ${FilterWidgets_BINARY_DIR}/FilterWidgetHeaders.h)
file(WRITE ${AllFilterWidgetsHeaderFile} "#ifndef _ALLFILTERWIDGETS_H_\n#define _ALLFILTERWIDGETS_H_\n")

# --------------------------------------------------------------------
# Create a File that will register all of the DREAM3D Filters
set(RegisterKnownFilterWidgetsFile ${FilterWidgets_BINARY_DIR}/RegisterKnownFilterWidgets.cpp)
#file(WRITE ${RegisterKnownFilterWidgetsFile} "#ifndef _RegisterKnownFilterWidgetsFile_H_\n#define _RegisterKnownFilterWidgetsFile_H_\n\n")
file(WRITE ${RegisterKnownFilterWidgetsFile} "#include \"FilterWidgets/QFilterWidgetManager.h\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"FilterWidgets/QFilterWidgetFactory.hpp\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"FilterWidgetHeaders.h\"\n")
#file(APPEND ${RegisterKnownFilterWidgetsFile} "\n\nnamespace DREAM3D { \nnamespace QFilterFactory {\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "\nvoid QFilterWidgetManager::RegisterKnownQFilterWidgets()\n{\n")

set(ONLY_FILTERS 1)
WidgetGeneration(GenericFilters)
WidgetGeneration(IOFilters)
WidgetGeneration(ManipulationFilters)
WidgetGeneration(ReconstructionFilters)
WidgetGeneration(StatisticsFilters)
WidgetGeneration(SurfaceMeshingFilters)
WidgetGeneration(SyntheticBuilderFilters)


file(APPEND ${AllFilterWidgetsHeaderFile} "\n#endif\n")

file(APPEND ${RegisterKnownFilterWidgetsFile} "\n }\n")
#file(APPEND ${RegisterKnownFilterWidgetsFile} "\n}\n}\n #endif")

# Read the file back into a string so we can configure our file
file(READ ${RegisterKnownFilterWidgetsFile} QFILTERWIDGET_FACTORY_SOURCE)
file(READ ${CodeGeneratorFile} CODE_GEN_FILTER_FUNCTION)
# Remove the temp files that we no longer need
file(REMOVE ${CodeGeneratorFile})

# --------------------------------------------------------------------            
# Create our custom executable that will generate most of our QFilterWidget
# classes from information stored in the Filters themselves. 
set(FilterWidgets_TEMP_DIR ${FilterWidgets_BINARY_DIR}/Temp)
file(MAKE_DIRECTORY ${FilterWidgets_BINARY_DIR}/Temp)
configure_file( ${FilterWidgets_SOURCE_DIR}/FilterWidgetsCodeGen.h.in
                ${FilterWidgets_BINARY_DIR}/FilterWidgetCodeGen.h)
add_executable(FilterWidgetCodeGen  ${FilterWidgets_SOURCE_DIR}/FilterWidgetCodeGen.cpp
                                    ${FilterWidgets_BINARY_DIR}/FilterWidgetCodeGen.h)
target_link_libraries(FilterWidgetCodeGen MXA EbsdLib DREAM3DLib)


# Now run the code to generate the header files which will over write the place
# holder files that were generated from above              
add_custom_command(TARGET FilterWidgetCodeGen POST_BUILD 
                    COMMAND $<TARGET_FILE:FilterWidgetCodeGen> )


# --------------------------------------------------------------------
# Set some Include directories
include_directories(${FilterWidgets_SOURCE_DIR}/..)
include_directories(${FilterWidgets_SOURCE_DIR})
include_directories(${FilterWidgets_BINARY_DIR})
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT4_WRAP_UI( FilterWidgets_Generated_UI_HDRS 
            ${FilterWidgets_SOURCE_DIR}/ReconstructionFiltersWidgets/QLoadSlicesWidget.ui 
            ${FilterWidgets_SOURCE_DIR}/SyntheticBuilderFiltersWidgets/QInitializeSyntheticVolumeWidget.ui)

# --------------------------------------------------------------------
# Add in ONLY those headers and sources that need to have various Qt related
# utilities run against them
set (QFilterWidget_HDRS
    ${FilterWidgets_SOURCE_DIR}/QFilterWidget.h
    ${FilterWidgets_SOURCE_DIR}/ReconstructionFiltersWidgets/QLoadSlicesWidget.h
    ${FilterWidgets_SOURCE_DIR}/ReconstructionFiltersWidgets/QualityMetricTableModel.h
    ${FilterWidgets_SOURCE_DIR}/ReconstructionFiltersWidgets/QualityMetricItemDelegate.h
    ${FilterWidgets_SOURCE_DIR}/SyntheticBuilderFiltersWidgets/QInitializeSyntheticVolumeWidget.h
)

set(QFilterWidget_SRCS
    ${FilterWidgets_SOURCE_DIR}/QFilterWidget.cpp
    ${FilterWidgets_SOURCE_DIR}/ReconstructionFiltersWidgets/QLoadSlicesWidget.cpp
    ${FilterWidgets_SOURCE_DIR}/ReconstructionFiltersWidgets/QualityMetricTableModel.cpp
    ${FilterWidgets_SOURCE_DIR}/SyntheticBuilderFiltersWidgets/QInitializeSyntheticVolumeWidget.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets" "${QFilterWidget_HDRS}" "${QFilterWidget_SRCS}" "0")
QT4_WRAP_CPP( FilterWidgets_MOC_SRCS ${QFilterWidget_HDRS} )


# --------------------------------------------------------------------
# and finally this will run moc:
QT4_WRAP_CPP( FilterWidgets_Generated_MOC_SRCS ${FilterWidget_GEN_HDRS})   

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt" "${FilterWidgets_Generated_UI_HDRS}" "${FilterWidgets_Generated_MOC_SRCS}" "0")

# --------------------------------------------------------------------
# These files are generated by our custom command
cmp_IDE_SOURCE_PROPERTIES( "Generated/FilterWidgetCodeGen" "${FilterWidget_GEN_HDRS}" "${FilterWidget_GEN_SRCS}" "0")

# --------------------------------------------------------------------
# Now add in any other headers or sources
set (FilterWidget_HDRS
    ${FilterWidgets_SOURCE_DIR}/IFilterWidgetFactory.h
    ${FilterWidgets_SOURCE_DIR}/QFilterWidgetFactory.hpp
    ${FilterWidgets_SOURCE_DIR}/QFilterWidgetManager.h
)

set(FilterWidget_SRCS
    ${FilterWidgets_SOURCE_DIR}/QFilterWidgetManager.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets" "${FilterWidget_HDRS}" "${FilterWidget_SRCS}" "0")


set (Project_SRCS 
        ${QFilterWidget_HDRS} 
        ${QFilterWidget_SRCS}
        ${FilterWidget_HDRS} 
        ${FilterWidget_SRCS}
        ${FilterWidget_GEN_HDRS}
        ${FilterWidget_GEN_SRCS}
        ${FilterWidgets_MOC_SRCS}
#        ${FilterWidgets_Generated_MOC_SRCS}
        ${FilterWidgets_Generated_UI_HDRS}
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
add_dependencies(FilterWidgets FilterWidgetCodeGen)
LibraryProperties( FilterWidgets ${EXE_DEBUG_EXTENSION})
