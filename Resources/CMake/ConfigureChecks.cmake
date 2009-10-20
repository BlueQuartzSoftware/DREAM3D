 # <distribution-statement>
 # #############################################################################
 # Distribution authorized to DOD components and DOD contractors only;         #
 # Administrative or Operational Use;                                          #
 # February 2009.                                                              #
 # Other requests for this document shall be referred to                       #
 # U.S. Air Force Research Laboratory                                          #
 # 2230 Tenth St., Ste. 1                                                      #
 # Wright-Patterson AFB, OH 45433-7817                                         #
 #                                                                             #
 # Destroy by contents or reconstruction of the document.                      #
 #                                                                             #
 # This code was written under the U.S. Air Force contrct F33615-03-C-5220     #
 # #############################################################################
 # </distribution-statement> 
 
# In this file we are doing all of our 'configure' checks. Things like checking
# for headers, functions, libraries, types and size of types.
INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckCXXSourceCompiles.cmake)

CHECK_INCLUDE_FILE("stddef.h" HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
set (SSE_COMPILE_FLAGS "")
option(AIM_USE_SSE "Use SSE2/3 Instructions where possible." OFF)
if (AIM_USE_SSE)
  if (NOT MSVC)
    set(CMAKE_REQUIRED_C_FLAGS_SAVE ${CMAKE_REQUIRED_C_FLAGS})
    set(CMAKE_REQUIRED_C_FLAGS ${CMAKE_REQUIRED_C_FLAGS} "-msse3")
    CHECK_INCLUDE_FILE("pmmintrin.h" HAVE_SSE3_H)
    set(CMAKE_REQUIRED_C_FLAGS ${CMAKE_REQUIRED_C_FLAGS_SAVE})
    if (HAVE_SSE3_H)
        set(HAVE_SSE2_H 1)
        if (CMAKE_COMPILER_IS_GNUCC)
          #  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse3")
          #  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3")
            set (SSE_COMPILE_FLAGS "-msse3")
        endif()
    else()
        set (HAVE_SSE3_H 0)
    endif()
    
    if (NOT HAVE_SSE3_H)
        set(CMAKE_REQUIRED_C_FLAGS_SAVE ${CMAKE_REQUIRED_C_FLAGS})
        set(CMAKE_REQUIRED_C_FLAGS ${CMAKE_REQUIRED_C_FLAGS} "-msse2")
        CHECK_INCLUDE_FILE("emmintrin.h" HAVE_SSE2_H)
        set(CMAKE_REQUIRED_C_FLAGS ${CMAKE_REQUIRED_C_FLAGS_SAVE})
        if(HAVE_SSE2_H)
            if (CMAKE_COMPILER_IS_GNUCC)
               # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2")
               # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
                set (SSE_COMPILE_FLAGS "-msse2")
            endif()
        else()
           set (HAVE_SSE2_H 0) 
        endif()
    endif()
  else()
    set(CMAKE_REQUIRED_C_FLAGS_SAVE ${CMAKE_REQUIRED_C_FLAGS})
    set(CMAKE_REQUIRED_C_FLAGS ${CMAKE_REQUIRED_C_FLAGS} "/arch:SSE2")
    CHECK_INCLUDE_FILE("intrin.h" HAVE_INTRIN_H)
    set(CMAKE_REQUIRED_C_FLAGS ${CMAKE_REQUIRED_C_FLAGS_SAVE})
    if (HAVE_INTRIN_H)
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:SSE2")
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
        set (SSE_COMPILE_FLAGS " /arch:SSE2 ")
    else()
        set (HAVE_INTRIN_H 0)
    endif()
  endif()
endif()


SET (VERSION_COMPILE_FLAGS "")

if (MSVC)
 set(HAVE_TIME_GETTIMEOFDAY "TRUE")
 set(VERSION_COMPILE_FLAGS "/DHAVE_TIME_GETTIMEOFDAY")
endif()
 
IF (NOT MSVC)
    TRY_COMPILE(HAVE_TIME_GETTIMEOFDAY
          ${CMAKE_BINARY_DIR}
          ${AIMRepresentation_RESOURCES_DIR}/CMake/GetTimeOfDayTest.cpp
          COMPILE_DEFINITIONS -DTRY_TIME_H
          OUTPUT_VARIABLE OUTPUT)
    IF (HAVE_TIME_GETTIMEOFDAY STREQUAL "TRUE")
        SET (HAVE_TIME_GETTIMEOFDAY "1")
        set(VERSION_COMPILE_FLAGS "-DHAVE_TIME_GETTIMEOFDAY")
    ENDIF (HAVE_TIME_GETTIMEOFDAY STREQUAL "TRUE")

    
    TRY_COMPILE(HAVE_SYS_TIME_GETTIMEOFDAY
          ${CMAKE_BINARY_DIR}
          ${AIMRepresentation_RESOURCES_DIR}/CMake/GetTimeOfDayTest.cpp
          COMPILE_DEFINITIONS -DTRY_SYS_TIME_H
          OUTPUT_VARIABLE OUTPUT)
    IF (HAVE_SYS_TIME_GETTIMEOFDAY STREQUAL "TRUE")
        SET (HAVE_SYS_TIME_GETTIMEOFDAY "1")
        set(VERSION_COMPILE_FLAGS "-DHAVE_SYS_TIME_GETTIMEOFDAY")
    ENDIF (HAVE_SYS_TIME_GETTIMEOFDAY STREQUAL "TRUE")

    if (NOT HAVE_SYS_TIME_GETTIMEOFDAY AND NOT HAVE_TIME_GETTIMEOFDAY)
        message(STATUS "---------------------------------------------------------------")
        message(STATUS "Function 'gettimeofday()' was not found. MXADataModel will use its")
        message(STATUS "  own implementation.. This can happen on older versions of")
        message(STATUS "  MinGW on Windows. Consider upgrading your MinGW installation")
        message(STATUS "  to a newer version such as MinGW 3.12")
        message(STATUS "---------------------------------------------------------------")
    endif()
    


ENDIF (NOT MSVC)

#-------------------------------------------------------------------------------
# This macro will attempt a try_run command in order to compile and then 
# generate a version string based on today's date. The output string should be
# of the form YYYY.MM.DD. 
#  Required CMake variables to be set are:
#   EmInit_CMAKE_DIR - The path to the MXA CMake directory
#  The following variables are set, all of which should have been already
#  initialized to a default value
#   ${PROJECT_NAME}_VERSION
#   ${PROJECT_NAME}_VER_MAJOR
#   ${PROJECT_NAME}_VER_MINOR
#   ${PROJECT_NAME}_VER_PATCH
#
#-------------------------------------------------------------------------------
macro(GenerateVersionString PROJECT_NAME)
#    message(STATUS "Generating Versions String for ${PROJECT_NAME}")
#    message(STATUS "EmInit_CMAKE_DIR: ${EmInit_CMAKE_DIR}")
    try_run(RUN_RESULT_VAR COMPILE_RESULT_VAR 
            ${CMAKE_CURRENT_BINARY_DIR} ${AIMRepresentation_RESOURCES_DIR}/CMake/GenerateVersionString.cpp
            COMPILE_DEFINITIONS ${VERSION_COMPILE_FLAGS}
            COMPILE_OUTPUT_VARIABLE VERSION_COMPILE_OUTPUT
            RUN_OUTPUT_VARIABLE ${PROJECT_NAME}_VERSION)
   
    # and now the version string given by qmake
    STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" ${PROJECT_NAME}_VER_MAJOR "${${PROJECT_NAME}_VERSION}")
    STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1" ${PROJECT_NAME}_VER_MINOR "${${PROJECT_NAME}_VERSION}")
    STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" ${PROJECT_NAME}_VER_PATCH "${${PROJECT_NAME}_VERSION}")
 
endmacro()
