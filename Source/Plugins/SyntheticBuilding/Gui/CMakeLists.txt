# Add in our Plugin Specific FilterParameterWidgets
include(${${PLUGIN_NAME}_SOURCE_DIR}/Gui/FilterParameterWidgets/SourceList.cmake)
include(${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/SourceList.cmake)
include(${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Utilities/SourceList.cmake)

set_source_files_properties( ${RegisterKnownFilterParameterWidgetsFile} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------------------------
# Create a Library that contains all the GUI/Widget codes that links
# to the Plugin
set(plug_gui_target_name ${PLUGIN_NAME}Gui)
add_library(${plug_gui_target_name} MODULE)
target_sources(${plug_gui_target_name}
  PRIVATE
    ${AllFilterParameterWidgetsHeaderFile}
    ${RegisterKnownFilterParameterWidgetsFile}
    
    ${${PLUGIN_NAME}_GEN_HDRS}


    # -- Add in the Filter Parameter Widgets
    ${${PLUGIN_NAME}_ParameterWidgets_HDRS}
    ${${PLUGIN_NAME}_ParameterWidgets_MOC_HDRS}
    ${${PLUGIN_NAME}_ParameterWidgets_SRCS}
    ${${PLUGIN_NAME}_ParameterWidgets_UIS}
    ${${PLUGIN_NAME}_ParameterWidgets_Generated_MOC_SRCS}
    ${${PLUGIN_NAME}_ParameterWidgets_Generated_UI_HDRS}

    # -- Add in the plain Widgets
     ${${PLUGIN_NAME}_Widgets_HDRS}
     ${${PLUGIN_NAME}_Widgets_SRCS}
     ${${PLUGIN_NAME}_Widgets_UIS}
     ${${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS}
     ${${PLUGIN_NAME}_Widgets_Generated_UI_HDRS}

     ${${PLUGIN_NAME}_Widgets_MISC_HDRS}

     # -- Add in Utility Sources
     ${${PLUGIN_NAME}_Utilities_HDRS}
     ${${PLUGIN_NAME}_Utilities_SRCS}
     ${${PLUGIN_NAME}_Utilities_MOC_HDRS}

     # -- Add in the StatsGenerator Sources
     ${${PLUGIN_NAME}_DELEGATE_SRCS}
     ${${PLUGIN_NAME}_DELEGATE_MOC_HDRS}
     ${${PLUGIN_NAME}_TABLEMODEL_SRCS}
     ${${PLUGIN_NAME}_TABLEMODEL_MOC_HDRS}
     ${${PLUGIN_NAME}_PRESETS_SRCS}
   
     ${${PLUGIN_NAME}_DIALOGS_SRCS}
     ${${PLUGIN_NAME}_DIALOGS_MOC_HDRS}
     ${${PLUGIN_NAME}_Generated_MOC_SRCS}
         
    # -- Add in the miscellaneous resources (license, plugin info, ...)
    ${${PLUGIN_NAME}_Generated_RC_SRCS}

    # -- Add in the QtPlugin definition source files for the Gui version of the plugin
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/${PLUGIN_NAME}GuiPlugin.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/${PLUGIN_NAME}GuiPlugin.h

)
CMP_AddDefinitions(TARGET ${plug_gui_target_name}) 
 
#------------------------------- 
# 
target_include_directories(${plug_gui_target_name}
                          PUBLIC
                            ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets
                            ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/FilterParameterWidgets
                            ${${PLUGIN_NAME}_BINARY_DIR}/Gui/Widgets
                            ${${PLUGIN_NAME}_BINARY_DIR}/Gui/FilterParameterWidgets
                            ${QWT_INCLUDE_DIR} 
)

# --------------------------------------------------------------------
# Set some additional properties of the plugin like its output name

PluginProperties(TARGET_NAME ${plug_gui_target_name}
                DEBUG_EXTENSION "_debug"
                VERSION "${DREAM3D_VERSION}"
                LIB_SUFFIX ".guiplugin"
                FOLDER ${PLUGIN_NAME}
                OUTPUT_NAME ${PLUGIN_NAME}Ui
                BINARY_DIR "${DREAM3D_BINARY_DIR}"
                PLUGIN_FILE "${CMP_PLUGIN_LIST_FILE}"
                INSTALL_DEST "./Plugins"
)

target_link_libraries(${plug_gui_target_name}
                        ${plug_target_name}
                        Qt5::Gui
                        Qt5::Widgets
                        SVWidgetsLib
                        ${QWT_LIBRARY}
)

# @END_OF_PLUGIN_GUI_CODE@
