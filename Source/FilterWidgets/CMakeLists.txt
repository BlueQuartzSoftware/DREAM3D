

project(FilterWidgets)
cmake_minimum_required(VERSION 2.8.4)


set(Project_SRCS "")
Macro(D3DCompileDir DirName)
    set(group ${DirName})
 #   file(APPEND ${AllFilterWidgetsHeaderFile} "\n\nnamespace Filters {\n namespace Options { \nconst std::string ${group}(\"${group}\");\n}\n}\n")
    file(APPEND ${CodeGeneratorFile} "//----- ${DirName} --------------- \n")
    # Make sure this directory is created.
    file(MAKE_DIRECTORY ${FilterWidgets_BINARY_DIR}/${DirName}Widgets)
    
    include(${PROJECT_CODE_DIR}/DREAM3DLib/${DirName}/SourceList.cmake)
  #  set (Project_SRCS ${Project_SRCS} ${DREAM3DLib_${DirName}_SRCS} ${DREAM3DLib_${DirName}_HDRS})
    
    file(APPEND ${FilterManagerConstructorFile}  "  {\n    std::set<std::string> l;\n")
 #   file(APPEND ${CodeGeneratorFile}  "  {\n    std::set<std::string> l;\n")
    
    foreach(file ${${DirName}_FILTERS_HDRS})
        get_filename_component(name ${file} NAME_WE)
        message(STATUS "    ${name}")
        if (NOT EXISTS ${FilterWidgets_SOURCE_DIR}/${DirName}Widgets/Q${name}Widget.h)
            configure_file(${FilterWidgets_SOURCE_DIR}/QFilterWidget.h.in
                          ${FilterWidgets_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
        
            set(FilterWidget_HDRS ${FilterWidget_HDRS} ${FilterWidgets_BINARY_DIR}/${DirName}Widgets/Q${name}Widget.h)
        endif()
        file(APPEND ${FilterManagerConstructorFile}   "    l.insert(${name}::ClassName());\n")
        file(APPEND ${CodeGeneratorFile} "createFile<${name}>(\"${DirName}\", \"${name}\");\n")
    
        file(APPEND ${FilterManagerFactoryFile} "   ${___ELSE} (filter.compare(${name}::ClassName()) == 0) {\n")
        file(APPEND ${FilterManagerFactoryFile} "     return new Q${name}Widget;\n")
        file(APPEND ${FilterManagerFactoryFile} "   }\n")
                if ( ${___ELSE} STREQUAL "if" )
                    set(___ELSE "else if")
                endif()
        file(APPEND ${AllFilterWidgetsHeaderFile} "#include \"FilterWidgets/${DirName}Widgets/Q${name}Widget.h\"\n")
        file(APPEND ${AllFilterWidgetsHeaderFile} "#include \"DREAM3DLib/${DirName}/${name}.h\"\n")
        
    endforeach()
    file(APPEND ${FilterManagerConstructorFile}  "    m_FilterMap[DREAM3D::FilterGroups::${DirName}] = l;\n  }\n")
 #   file(APPEND ${CodeGeneratorFile} "    m_FilterMap[\"${DirName}\"] = l;\n  }\n")
endMacro()

set(___ELSE "if")
set (CodeGeneratorFile  ${FilterWidgets_BINARY_DIR}/CodeGenerator.h)
file(WRITE ${CodeGeneratorFile} "")

set (AllFilterWidgetsHeaderFile ${FilterWidgets_BINARY_DIR}/DREAM3DFilters.h)
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
    
if (1)  
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
    FILE(READ ${CodeGeneratorFile} CODE_GEN_FILTER_FUNCTION)
    FILE(REMOVE ${FilterManagerConstructorFile})
    FILE(REMOVE ${FilterManagerFactoryFile})
    FILE(REMOVE ${CodeGeneratorFile})
endif()


set(FILTERMANAGER_CLASS_NAME "QDream3DFilterManager")
configure_file(${FilterWidgets_SOURCE_DIR}/FilterManager.h.in
            ${FilterWidgets_BINARY_DIR}/${FILTERMANAGER_CLASS_NAME}.h)
            
configure_file( ${FilterWidgets_SOURCE_DIR}/CodeGen.cpp.in
                ${FilterWidgets_BINARY_DIR}/FilterWidgetCodeGen.cpp)
add_executable(FilterWidgetCodeGen ${FilterWidgets_BINARY_DIR}/FilterWidgetCodeGen.cpp)
target_link_libraries(FilterWidgetCodeGen MXA EbsdLib DREAM3DLib)
#add_custom_command(OUTPUT FilterWidgetCodeGen_File_List 
#                    COMMAND FilterWidgetCodeGen 
#                    MAIN_DEPENDENCY  ${FilterWidgets_SOURCE_DIR}/CodeGen.cpp.in
#                    WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(EXE_EXTENSION "")
if (WIN32)
  set (EXE_EXTENSION ".exe")
endif()                   
add_custom_command(TARGET FilterWidgetCodeGen POST_BUILD 
            COMMAND FilterWidgetCodeGen${EXE_EXTENSION} 
            WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set (FilterWidget_HDRS
    ${FilterWidget_HDRS}
    ${FilterWidgets_SOURCE_DIR}/ReconstructionFiltersWidgets/QLoadSlicesWidget.h
)


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
add_dependencies(FilterWidgets FilterWidgetCodeGen)
LibraryProperties( FilterWidgets ${EXE_DEBUG_EXTENSION})
