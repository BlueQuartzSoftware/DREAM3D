

#-------------------------------------------------------------------------------
# Macro START_FILTER_GROUP
macro(START_FILTER_GROUP WidgetsBinaryDir filterGroup)
    file(APPEND ${AllFiltersHeaderFile} "\n/* ------ ${filterGroup} --------- */\n")
    file(APPEND ${CodeGeneratorFile} "//----- ${filterGroup} --------------- \n")
    file(APPEND ${HtmlDocQrcFile} "\n    <!-- ***** ${filterGroup} ***** -->\n")
    file(APPEND ${AllFilterWidgetsHeaderFile} "\n/* ------ ${filterGroup} --------- */\n")
    file(APPEND ${RegisterKnownFilterWidgetsFile} "\n    /* ------ ${filterGroup} --------- */\n")
 #   file(MAKE_DIRECTORY ${${WidgetLib}_BINARY_DIR}/${filterGroup}Widgets)
    message(STATUS "Generating Widgets for ${filterGroup}")
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_SUPPORT_HEADER
macro(ADD_DREAM3D_SUPPORT_HEADER SourceDir filterGroup headerFileName)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${headerFileName})
    cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/${filterGroup}" "${DREAM3DLib_SOURCE_DIR}/${filterGroup}/${headerFileName}" "" "0")
endmacro()
#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_SUPPORT_SOURCE
macro(ADD_DREAM3D_SUPPORT_SOURCE SourceDir filterGroup sourceFileName)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${sourceFileName})
    cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/${filterGroup}" "" "${DREAM3DLib_SOURCE_DIR}/${filterGroup}/${sourceFileName}" "0")
endmacro()
#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_SUPPORT_CLASS
macro(ADD_DREAM3D_SUPPORT_CLASS SourceDir filterGroup className)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${className}.h
                    ${SourceDir}/${filterGroup}/${className}.cpp)
    cmp_IDE_SOURCE_PROPERTIES( "DREAM3DLib/${filterGroup}" "${DREAM3DLib_SOURCE_DIR}/${filterGroup}/${className}.h" "${DREAM3DLib_SOURCE_DIR}/${filterGroup}/${className}.cpp" "0")
endmacro()


#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_FILTER
macro(ADD_DREAM3D_FILTER FilterLib WidgetLib filterGroup filterName filterDocFile filterDocImages publicFilter)

    set(Project_SRCS ${Project_SRCS}
                    ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h
                    ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.cpp)
    #--- Organize inside the Visual Studio/Xcode Projects
    cmp_IDE_SOURCE_PROPERTIES( "${FilterLib}/${filterGroup}" "${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h" "${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.cpp" "0")

    #-- Create an Install Rule for the headers
    if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
        INSTALL (FILES ${DREAM3DLib_SOURCE_DIR}/${filterGroup}/${filterName}.h
            DESTINATION include/DREAM3DLib/${filterGroup}
            COMPONENT Headers   )
    endif()

    file(APPEND ${AllFiltersHeaderFile} "#include \"${FilterLib}/${filterGroup}/${filterName}.h\"\n")

    if( ${publicFilter} STREQUAL TRUE)
        message(STATUS "    ${filterName}")
        file(APPEND ${CodeGeneratorFile} "  ${filterName}::Pointer _${filterName} = ${filterName}::New();\n")

        if(NOT EXISTS ${${WidgetLib}_SOURCE_DIR}/${FilterLib}/${filterGroup}Widgets/Q${filterName}Widget.h )
          #  message(STATUS "${${WidgetLib}_SOURCE_DIR}/${FilterLib}/${filterGroup}Widgets/Q${filterName}Widget.h")
            file(APPEND ${FilterWidget_GEN_HDRS_File} "${${WidgetLib}_BINARY_DIR}/${FilterLib}/${filterGroup}Widgets/Q${filterName}Widget.h;")
            file(APPEND ${FilterWidget_GEN_SRCS_File} "${${WidgetLib}_BINARY_DIR}/${FilterLib}/${filterGroup}Widgets/Q${filterName}Widget.cpp;")
            if( NOT EXISTS ${${WidgetLib}_BINARY_DIR}/${FilterLib}/${filterGroup}Widgets/Q${filterName}Widget.h)
                set(GENERATED_MOC_SOURCE_FILE "moc_Q${name}Widget.cpp")
                configure_file(${FilterWidgetsLib_SOURCE_DIR}/QFilterWidget_Template.h.in
                              ${${WidgetLib}_BINARY_DIR}/${FilterLib}/${filterGroup}Widgets/Q${filterName}Widget.h)
                configure_file(${FilterWidgetsLib_SOURCE_DIR}/QFilterWidget_Template.cpp.in
                              ${${WidgetLib}_BINARY_DIR}/${FilterLib}/${filterGroup}Widgets/Q${filterName}Widget.cpp)
            endif()

            file(APPEND ${CodeGeneratorFile} "  createHeaderFile(\"${filterGroup}\", \"${filterName}\", _${filterName}->getFilterParameters());\n")
            file(APPEND ${CodeGeneratorFile} "  createSourceFile(\"${filterGroup}\", \"${filterName}\", _${filterName}->getFilterParameters());\n")
        endif()

        file(APPEND ${CodeGeneratorFile} "  createHTMLFile(\"${filterGroup}\", \"${filterName}\", _${filterName}.get());\n\n")

        file(APPEND ${AllFilterWidgetsHeaderFile} "#include \"${FilterLib}/${filterGroup}Widgets/Q${filterName}Widget.h\"\n")

        file(APPEND ${HtmlDocQrcFile} "    <file>${filterGroup}/${filterName}.html</file>\n")
        foreach(resource ${filterDocImages})
            file(APPEND ${HtmlDocQrcFile} "    <file>${filterGroup}/${resource}</file>\n")
        endforeach()

        file(APPEND ${RegisterKnownFilterWidgetsFile} "   QFilterWidgetFactory<Q${filterName}Widget>::Pointer q${filterName}WidgetFactory = QFilterWidgetFactory<Q${filterName}Widget>::New(); \n")
        file(APPEND ${RegisterKnownFilterWidgetsFile} "   FilterWidgetManager::Instance()->addFilterWidgetFactory(\"${filterName}\",q${filterName}WidgetFactory); \n\n")

    endif()
endmacro()
