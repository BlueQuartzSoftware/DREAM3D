message(FATAL_ERROR "THIS FILE IS NO LONGER USED")



# -------- Function to build OS X Stand Alone Bundles -----------------
macro(MakeOSXBundleApp target binary_dir osx_tools_dir)
    set(target ${target})
    set(binary_dir ${binary_dir})
    set(osx_tools_dir ${osx_tools_dir})
    
    set (OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT "${binary_dir}/OSX_Scripts/${target}_OSX_MakeStandAloneBundle.cmake")
    
    CONFIGURE_FILE("${osx_tools_dir}/CompleteBundle.cmake.in"
            "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
     
    install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}")
                    
endmacro(MakeOSXBundleApp)


# -------- Function to build OS X Stand Alone Bundles -----------------
macro(MakeOSXTool target binary_dir osx_tools_dir)
    set(target ${target})
    set(binary_dir ${binary_dir})
    set(osx_tools_dir ${osx_tools_dir})
    
    set (OSX_MAKE_STANDALONE_TOOL_CMAKE_SCRIPT 
        "${binary_dir}/OS_X_Scripts/${target}_OSX_MakeStandAloneTool.cmake")
    
    CONFIGURE_FILE("${osx_tools_dir}/CompleteTool.cmake.in"
            "${OSX_MAKE_STANDALONE_TOOL_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
     
    install(SCRIPT "${OSX_MAKE_STANDALONE_TOOL_CMAKE_SCRIPT}")
                    
endmacro(MakeOSXTool)





