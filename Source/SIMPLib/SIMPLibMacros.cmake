

#-------------------------------------------------------------------------------
# Macro START_FILTER_GROUP
macro(START_FILTER_GROUP WidgetsBinaryDir filterGroup humanGroup)
   file(APPEND ${AllFiltersHeaderFile} "\n/* ------ ${filterGroup} --------- */\n")
 #  file(APPEND ${CodeGeneratorFile} "//----- ${filterGroup} --------------- \n")
 #  file(APPEND ${AllFilterWidgetsHeaderFile} "\n/* ------ ${filterGroup} --------- */\n")
 #  file(APPEND ${RegisterKnownFilterWidgetsFile} "\n    /* ------ ${filterGroup} --------- */\n")
   file(APPEND ${RegisterKnownFiltersFile} "\n    /* ------ ${filterGroup} --------- */\n")

   file(APPEND "${DREAM3DProj_BINARY_DIR}/DREAM3DDocGroupList" "${filterGroup}\n")
   file(WRITE "${DREAM3DProj_BINARY_DIR}/DREAM3DDoc_${filterGroup}" "")
#   FILE(WRITE ${PluginAutoMocSourceFile} "/* This file is Auto Generated. Do Not Edit */\n")
endmacro()

#-------------------------------------------------------------------------------
# Macro END_FILTER_GROUP
macro(END_FILTER_GROUP WidgetsBinaryDir filterGroup humanGroup)
#    file(APPEND ${DREAM3DProj_SOURCE_DIR}/Source/Applications/DREAM3D/Help/Filters/${filterGroup}/${filterGroup}.dox "\n*/\n")
endmacro(END_FILTER_GROUP  WidgetsBinaryDir filterGroup)


macro(MAKE_CONSTANTS_HEADER PLUGIN_NAME)
  set(PLUGIN_NAME ${PLUGIN_NAME})
  set(ConstantsFile ${${PLUGIN_NAME}_SOURCE_DIR}/${PLUGIN_NAME}Constants.h)
  file(WRITE ${ConstantsFile} "#ifndef _${PLUGIN_NAME}_H_\n#define _${PLUGIN_NAME}_H_\n\n#include <QtCore/QString>\n\n")
  file(APPEND ${ConstantsFile} "namespace ${PLUGIN_NAME} {\n")
  file(APPEND ${ConstantsFile} "  const QString ${PLUGIN_NAME}PluginFile(\"${PLUGIN_NAME}Plugin\");\n")
  file(APPEND ${ConstantsFile} "  const QString ${PLUGIN_NAME}PluginDisplayName(\"${PLUGIN_NAME}Plugin\");\n")
  file(APPEND ${ConstantsFile} "  const QString ${PLUGIN_NAME}PluginBaseName(\"${PLUGIN_NAME}Plugin\");\n")
  file(APPEND ${ConstantsFile} "}\n")
  file(APPEND ${ConstantsFile} "#endif\n")
endmacro()


#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_SUPPORT_HEADER
macro(ADD_DREAM3D_SUPPORT_HEADER SourceDir filterGroup headerFileName)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${headerFileName})
    cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "${SourceDir}/${filterGroup}/${headerFileName}" "" "0")
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_SUPPORT_MOC_HEADER
macro(ADD_DREAM3D_SUPPORT_MOC_HEADER SourceDir filterGroup headerFileName)
  QT5_WRAP_CPP( _moc_filter_source  ${SourceDir}/${filterGroup}/${headerFileName})
  set_source_files_properties( ${_moc_filter_source} PROPERTIES GENERATED TRUE)
  set_source_files_properties( ${_moc_filter_source} PROPERTIES HEADER_FILE_ONLY TRUE)

  set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${headerFileName}
                    ${_moc_filter_source})
  cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "${SourceDir}/${filterGroup}/${headerFileName}" "" "0")
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_SUPPORT_HEADER_SUBDIR
macro(ADD_DREAM3D_SUPPORT_HEADER_SUBDIR SourceDir filterGroup headerFileName subdir)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${subdir}/${headerFileName})
    cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}/${subdir}" "${SourceDir}/${filterGroup}/${subdir}/${headerFileName}" "" "0")
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_SUPPORT_SOURCE
macro(ADD_DREAM3D_SUPPORT_SOURCE SourceDir filterGroup sourceFileName)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${sourceFileName})
    cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "" "${SourceDir}/${filterGroup}/${sourceFileName}" "0")
endmacro()
#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_SUPPORT_CLASS
macro(ADD_DREAM3D_SUPPORT_CLASS SourceDir filterGroup className)
    set(Project_SRCS ${Project_SRCS}
                    ${SourceDir}/${filterGroup}/${className}.h
                    ${SourceDir}/${filterGroup}/${className}.cpp)
    cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "${SourceDir}/${filterGroup}/${className}.h" "${SourceDir}/${filterGroup}/${className}.cpp" "0")
endmacro()


#-------------------------------------------------------------------------------
# Macro ADD_DREAM3D_FILTER
macro(ADD_DREAM3D_FILTER FilterLib WidgetLib filterGroup filterName filterDocPath publicFilter)
    QT5_WRAP_CPP( _moc_filter_source  ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h)
    set_source_files_properties( ${_moc_filter_source} PROPERTIES GENERATED TRUE)
    set_source_files_properties( ${_moc_filter_source} PROPERTIES HEADER_FILE_ONLY TRUE)

    set(Project_SRCS ${Project_SRCS}
                    ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h
                    ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.cpp
                    ${_moc_filter_source}
                    )
    #--- Organize inside the Visual Studio/Xcode Projects
    cmp_IDE_SOURCE_PROPERTIES( "${filterGroup}" "${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h" "${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.cpp" "0")
    cmp_IDE_GENERATED_PROPERTIES ( "Generated/${FilterLib}/${filterGroup}" "" "${_moc_filter_source}" "0")

    #-- Create an Install Rule for the headers
    if( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
        INSTALL (FILES ${${FilterLib}_SOURCE_DIR}/${filterGroup}/${filterName}.h
            DESTINATION include/${filterGroup}
            COMPONENT Headers   )
    endif()

    file(APPEND ${AllFiltersHeaderFile} "#include \"${FilterLib}/${filterGroup}/${filterName}.h\"\n")

    if( ${publicFilter} STREQUAL TRUE)
        #message(STATUS "    ${filterName}")
        #file(APPEND ${CodeGeneratorFile} "  ${filterName}::Pointer _${filterName} = ${filterName}::New();\n")
        file(APPEND ${RegisterKnownFiltersFile} "   FilterFactory<${filterName}>::Pointer ${filterName}Factory = FilterFactory<${filterName}>::New();\n")
        file(APPEND ${RegisterKnownFiltersFile} "   fm->addFilterFactory(\"${filterName}\",${filterName}Factory);\n\n")

       #-- Check to make sure we have a Documentation file for the filter
        if(NOT EXISTS ${filterDocPath} )
          message(FATAL_ERROR "*** Missing Documenation File for ${filterDocPath}")
        endif()

        file(APPEND ${DREAM3DProj_BINARY_DIR}/DREAM3DDoc_${filterGroup} "${filterDocPath}\n")

    endif()
endmacro()

#-------------------------------------------------------------------------------
# Macro ADD_FILTER_LIST
macro(ADD_FILTER_LIST)

        file(APPEND ${RegisterKnownFiltersFile} "\tQList<QString> pluginList;\n\n")

    foreach(f ${_PublicFilters} )
        file(APPEND ${RegisterKnownFiltersFile} "\tpluginList.append(\"${f}\");\n")
    endforeach()

    file(APPEND ${RegisterKnownFiltersFile} "\n\treturn pluginList;")

endmacro()
