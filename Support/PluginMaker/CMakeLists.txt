#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////
project(PluginMaker NONE)

set(DREAM3D_Plugin_Name "MyClass" CACHE STRING "The Name of the new Pipeline Class" )
set(PluginName ${DREAM3D_Plugin_Name})

if (1)
    set(PluginMaker_PluginDir ${DREAM3D_BINARY_DIR}/PluginMaker/${PluginName}/UI)
    set(PluginMaker_LibDir ${DREAM3D_BINARY_DIR}/PluginMaker/${PluginName})
else()
    # If this gets enabled then the new source files will be DIRECTLY written to the
    # source directory which means anything that has the same name that you have selected
    # WILL BE OVERWRITTEN without warning. You have been warned. This is here for 
    # debugging this build system and should NOT be enabled for general use.
    set(PluginMaker_PluginDir ${DREAM3D_SOURCE_DIR}/${DREAM3D_Plugin_Name}/UI)
    set(PluginMaker_LibDir ${DREAM3D_SOURCE_DIR}/${DREAM3D_Plugin_Name})
endif()

# Create a directory for the Plugin Files
file(MAKE_DIRECTORY ${PluginMaker_PluginDir})
file(MAKE_DIRECTORY ${PluginMaker_LibDir})

#Configure the Files that will be added to the DREAM3D Library
configure_file(Templates/Plugins/PluginMaker.cpp.in
    ${PluginMaker_LibDir}/${DREAM3D_Plugin_Name}.cpp)
configure_file(Templates/Plugins/PluginMaker.h.in
    ${PluginMaker_LibDir}/${DREAM3D_Plugin_Name}.h)
configure_file(Templates/Plugins/PluginMakerFunc.cpp.in
    ${PluginMaker_LibDir}/${DREAM3D_Plugin_Name}Func.cpp)
configure_file(Templates/Plugins/PluginMakerFunc.h.in
    ${PluginMaker_LibDir}/${DREAM3D_Plugin_Name}Func.h)


#Configure the files that will be turned into the UI Plugin
configure_file( Templates/Plugins/UI/PluginMakerPlugin.h.in
${PluginMaker_PluginDir}/${DREAM3D_Plugin_Name}Plugin.h @ONLY IMMEDIATE)

configure_file( Templates/Plugins/UI/PluginMakerPlugin.cpp.in
${PluginMaker_PluginDir}/${DREAM3D_Plugin_Name}Plugin.cpp @ONLY IMMEDIATE)

configure_file( Templates/Plugins/UI/PluginMakerWidget.h.in
${PluginMaker_PluginDir}/${DREAM3D_Plugin_Name}Widget.h @ONLY IMMEDIATE)

configure_file( Templates/Plugins/UI/PluginMakerWidget.cpp.in 
${PluginMaker_PluginDir}/${DREAM3D_Plugin_Name}Widget.cpp @ONLY IMMEDIATE)

configure_file( Templates/Plugins/CMakeLists.txt.in  
${PluginMaker_LibDir}/CMakeLists.txt @ONLY IMMEDIATE)

configure_file( Templates/Plugins/UI/PluginMaker.png  
${PluginMaker_PluginDir}/${DREAM3D_Plugin_Name}.png COPYONLY)

configure_file( Templates/Plugins/UI/PluginMakerWidget.ui.in 
${PluginMaker_PluginDir}/${DREAM3D_Plugin_Name}Widget.ui @ONLY IMMEDIATE)

configure_file( Templates/Plugins/UI/Icons.qrc.in  
${PluginMaker_PluginDir}/Icons.qrc @ONLY IMMEDIATE)

configure_file( Templates/Plugins/UI/Help/index.html.in  
${PluginMaker_PluginDir}/Help/index.html @ONLY IMMEDIATE)


message(STATUS "You MUST Move the new Plguin directory into the proper spot in the actual Source directory")
message(STATUS "located at ${PROJECT_CODE_DIR}/Plugins")
message(STATUS "You also may want to create your own custom toolbar icon and store")
message(STATUS "it in the ${DREAM3D_Plugin_Name}.png file")
