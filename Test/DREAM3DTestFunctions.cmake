


# --------------------------------------------------------------------------
# Converts file paths to use the '/' character so they are compatible with
# C/C++ language. The use of the "\" character would make the compiler think
# the following character would be escaped.
#-- Convert all '\' to '\\' so that they are properly escaped in the header file
macro(ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()


function(AddDREAM3DUnitTest)
    set(options)
    set(oneValueArgs TESTNAME FOLDER)
    set(multiValueArgs SOURCES LINK_LIBRARIES)
    cmake_parse_arguments(Z "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    add_executable( ${Z_TESTNAME} "${Z_SOURCES}")
    target_link_libraries( ${Z_TESTNAME} ${Z_LINK_LIBRARIES})
    if("${Z_FOLDER}" STREQUAL "")
        set(Z_FOLDER "Test")
    endif()
    set_target_properties( ${Z_TESTNAME} PROPERTIES FOLDER ${Z_FOLDER})
    cmp_IDE_SOURCE_PROPERTIES( "" "" "${Z_SOURCES}" "0")

    add_test(${Z_TESTNAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${Z_TESTNAME})

endfunction()

