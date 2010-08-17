#///////////////////////////////////////////////////////////////////////////////
#//
#//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
#//  All rights reserved.
#//  BSD License: http://www.opensource.org/licenses/bsd-license.html
#//
#///////////////////////////////////////////////////////////////////////////////
# ------------------------------------------------------------------------------ 
# This CMake code installs the needed support libraries
# ------------------------------------------------------------------------------ 

macro(InstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

    SET_TARGET_PROPERTIES( ${EXE_NAME} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
        RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    
    
    INSTALL(TARGETS ${EXE_NAME} 
        RUNTIME
        DESTINATION ./
        COMPONENT Applications
        LIBRARY DESTINATION ./ 
        ARCHIVE DESTINATION ./
        RUNTIME DESTINATION ./
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/.
    )   
    
    # --- If we are on OS X copy all the embedded libraries to the app bundle
    if(DEFINED GUI_TYPE)
       # message(STATUS "Creating Install CMake file for GUI application ${EXE_NAME}")
        if(${GUI_TYPE} STREQUAL "MACOSX_BUNDLE")
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif()
    else(DEFINED GUI_TYPE)
      #  message(STATUS "Creating Install CMake file for tool application ${EXE_NAME}")
        if (APPLE)
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif(APPLE)
    endif(DEFINED GUI_TYPE)
endmacro(InstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)


macro(ToolInstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

    SET_TARGET_PROPERTIES( ${EXE_NAME} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
        RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    
    if (APPLE)
        set(TOOL_INSTALL_LOCATION "bin")
    else()
        set(TOOL_INSTALL_LOCATION "./")
    endif()
    
    INSTALL(TARGETS ${EXE_NAME} 
        RUNTIME
        DESTINATION ${TOOL_INSTALL_LOCATION}
        COMPONENT Tools
        LIBRARY DESTINATION ${TOOL_INSTALL_LOCATION} 
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION ${TOOL_INSTALL_LOCATION}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/.
    )   
    
    # --- If we are on OS X copy all the embedded libraries to the app bundle
    if(DEFINED GUI_TYPE)
      #  message(STATUS "Creating Install CMake file for GUI application ${EXE_NAME}")
        if(${GUI_TYPE} STREQUAL "MACOSX_BUNDLE")
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif()
    else(DEFINED GUI_TYPE)
     #   message(STATUS "Creating Install CMake file for tool application ${EXE_NAME}")
        if (APPLE)
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif(APPLE)
    endif(DEFINED GUI_TYPE)
endmacro(ToolInstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

