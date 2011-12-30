

project(FilterWidgets)
cmake_minimum_required(VERSION 2.8.4)


set(Project_SRCS "")
Macro(D3DCompileDir DirName)
    set(group ${DirName})
    include(${PROJECT_CODE_DIR}/DREAM3DLib/${DirName}/SourceList.cmake)
  #  set (Project_SRCS ${Project_SRCS} ${DREAM3DLib_${DirName}_SRCS} ${DREAM3DLib_${DirName}_HDRS})

    file(APPEND ${FilterManagerConstructorFile}  "  {\n    std::set<std::string> l;\n")
    message(STATUS "${DirName}")
    foreach(file ${${DirName}_FILTERS_HDRS})
        get_filename_component(name ${file} NAME_WE)
        message(STATUS "    ${name}")
        configure_file(${FilterWidgets_SOURCE_DIR}/QFilterWidget.h.in
                       ${FilterWidgets_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
        set(FilterWidget_HDRS ${FilterWidget_HDRS} ${FilterWidgets_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
        file(APPEND ${FilterManagerConstructorFile}   "    l.insert(\"${name}\");\n")
        file(APPEND ${FilterManagerFactoryFile} "   if (filter.compare(\"${name}\") == 0) {\n")
        file(APPEND ${FilterManagerFactoryFile} "     return new Q${name}Widget;\n")
        file(APPEND ${FilterManagerFactoryFile} "   }\n")
        file(APPEND ${AllFilterWidgetsHeaderFile} "#include \"FilterWidgets/${DirName}Widgets/Q${name}Widget.h\"\n")
    endforeach()
    file(APPEND ${FilterManagerConstructorFile}  "    m_FilterMap[\"${DirName}\"] = l;\n  }\n")

endMacro()

set (AllFilterWidgetsHeaderFile ${FilterWidgets_BINARY_DIR}/AllFilterWidgets.h)
file(WRITE ${AllFilterWidgetsHeaderFile} "#ifndef _ALLFILTERWIDGETS_H_\n#define _ALLFILTERWIDGETS_H_\n")

set(FilterManagerConstructorFile ${FilterWidgets_BINARY_DIR}/FilterManager.h)
file(WRITE ${FilterManagerConstructorFile} "" )

set(FilterManagerFactoryFile ${FilterWidgets_BINARY_DIR}/FilterFactory.h)

set(FilterWidget_HDRS
    ${FilterWidgets_SOURCE_DIR}/QFilterWidget.h
    )
set(FilterWidget_SRCS
    ${FilterWidgets_SOURCE_DIR}/QFilterWidget.cpp
    )
    
D3DCompileDir(GenericFilters)
D3DCompileDir(ReconstructionFilters)
D3DCompileDir(ManipulationFilters)
D3DCompileDir(StatisticsFilters)
D3DCompileDir(SyntheticBuilderFilters)
D3DCompileDir(SurfaceMeshingFilters)


file(APPEND ${AllFilterWidgetsHeaderFile} "\n#endif\n")

# Read the file back into a string so we can configure our file
FILE(READ ${FilterManagerConstructorFile} FILTERMANAGER_CONSTRUCTOR_SOURCE)
FILE(READ ${FilterManagerFactoryFile} QFILTERWIDGET_FACTORY_SOURCE)
FILE(REMOVE ${FilterManagerConstructorFile})
FILE(REMOVE ${FilterManagerFactoryFile})
set(FILTERMANAGER_CLASS_NAME "QDream3DFilterManager")
configure_file(${FilterWidgets_SOURCE_DIR}/FilterManager.h.in
            ${FilterWidgets_BINARY_DIR}/${FILTERMANAGER_CLASS_NAME}.h)

            
# --------------------------------------------------------------------
# and finally this will run moc:
QT4_WRAP_CPP( FilterWidgets_Generated_MOC_SRCS ${FilterWidget_HDRS} )   
 # --------------------------------------------------------------------
#-- Put the generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated" "" "${FilterWidgets_Generated_MOC_SRCS}" "0")

cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets" "${FilterWidget_HDRS}" "${FilterWidget_SRCS}" "0")

set (Project_SRCS 
        ${FilterWidget_HDRS} 
        ${FilterWidget_SRCS}
        ${FilterWidgets_Generated_MOC_SRCS}
        )
        

if (UNIX AND NOT APPLE)
set_source_files_properties(${Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC) 
endif()

add_library(FilterWidgets STATIC ${Project_SRCS})
target_link_libraries(FilterWidgets 
                    ${QT_QTCORE_LIBRARY} 
                    ${QT_QTGUI_LIBRARY}
                    DREAM3DLib
                    )

LibraryProperties( FilterWidgets ${EXE_DEBUG_EXTENSION})
