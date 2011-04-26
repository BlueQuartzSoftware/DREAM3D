# - Try to find Boost
# Once done this will define
#
#  Boost_FOUND - System has Boost
#  Boost_INCLUDE_DIRS - Boost include directory
#  Boost_LIBRARIES - Link these to use Boost
#  Boost_LIBRARY_DIRS - The path to where the Boost library files are.
#  Boost_DEFINITIONS - Compiler switches required for using Boost
#  Boost_LIBRARIES_SUFFIX - Boost libraries name suffix (e.g -vc71-mt-gd-1_34, -gcc41-mt...)
#
#  Boost_CURRENT_VERSION  The version of Boost that you want to find (1_34_1)
#  Boost_COMPILER      Boost's abbreviation for the compiler used. Reasonable defaults have
#   been set for Windows Visual Studio (2003, 2003.net and VS8), MinGW, Cygwin, 
#   Apple OS X and Intel Compiler on Linux. If you need another combination set this
#   cmake variable PRIOR to including this file in your cmakelists.txt.
#   
#  Environment Variables:
#   BOOSTLIBDIR  The directory where the libraries are located
#   BOOST_ROOT   The Directory where boost is installed. 
#
#  Use these variables to control which libraries will be used for linking.
#  Boost_USE_DATE_TIME     Link against the date_time Library
#  Boost_USE_FILESYSTEM     Link against the filesystem Library
#  Boost_USE_IOSTREAMS     Link against the iostreams Library
#  Boost_USE_PRO_EXEC_MONITOR     Link against the prg_exec_monitor Library
#  Boost_USE_PROGRAM_OPTIONS     Link against the program_options Library
#  Boost_USE_PYTHON     Link against the python Library
#  Boost_USE_REGEX     Link against the regex Library
#  Boost_USE_SERIALIZATION     Link against the serialization Library
#  Boost_USE_SIGNALS     Link against the signals Library
#  Boost_USE_TEST_EXEC_MONITOR     Link against the test_exec_monitor Library
#  Boost_USE_THREAD     Link against the thread Library
#  Boost_USE_UNIT_TEST_FRAMEWORK     Link against the unit_test_framework Library
#  Boost_USE_WSERIALIZATION     Link against the wserialization Library
#  Boost_USE_SYSTEM     Link against the boost::system Library
#
#  Boost_DATE_TIME_FOUND               True IF Boost Date Time was found.
#  Boost_FILESYSTEM_FOUND              True IF Boost Filesystem was found.
#  Boost_IOSTREAMS_FOUND               True IF Boost Iostream was found.
#  Boost_PRG_EXEC_MONITOR_FOUND        True IF Boost Program Exec Monitor was found.
#  Boost_PROGRAM_OPTIONS_FOUND         True IF Boost Program Options was found.
#  Boost_PYTHON_FOUND                  True IF Boost Python was found.
#  Boost_REGEX_FOUND                   True IF Boost Regex was found.
#  Boost_SERIALIZATION_FOUND           True IF Boost Serialization was found.
#  Boost_SIGNALS_FOUND                 True IF Boost Signals was found.
#  Boost_TEST_EXEC_MONITOR_FOUND       True IF Boost Test Exec Monitor was found.
#  Boost_THREAD-MT_FOUND               True IF Boost Thread was found.
#  Boost_UNIT_TEST_FRAMEWORK_FOUND     True IF Boost Unit Test Framework was found.
#  Boost_WSERIALIZATION_FOUND          True IF Boost WSerialization was found.
#  Boost_SYSTEM_FOUND                  True IF Boost System was found.
#
#  Boost_DATE_TIME_LIBRARY     The Boost date_time libraries
#  Boost_DATE_TIME_LIBRARY_DEBUG     The Boost date_time debug library
#  Boost_DATE_TIME_LIBRARY_RELEASE   The Boost date_time release library
#
#  Boost_FILESYSTEM_LIBRARY     The Boost filesystem libraries
#  Boost_FILESYSTEM_LIBRARY_DEBUG     The Boost filesystem debug library
#  Boost_FILESYSTEM_LIBRARY_RELEASE   The Boost filesystem release library
#
#  Boost_IOSTREAMS_LIBRARY     The Boost iostreams libraries
#  Boost_IOSTREAMS_LIBRARY_DEBUG     The Boost iostreams debug library
#  Boost_IOSTREAMS_LIBRARY_RELEASE   The Boost iostreams release library
#
#  Boost_PRO_EXEC_MONITOR_LIBRARY     The Boost prg_exec_monitor libraries
#  Boost_PRO_EXEC_MONITOR_LIBRARY_DEBUG     The Boost prg_exec_monitor debug library
#  Boost_PRO_EXEC_MONITOR_LIBRARY_RELEASE   The Boost prg_exec_monitor release library
#
#  Boost_PROGRAM_OPTIONS_LIBRARY     The Boost program_options libraries
#  Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG     The Boost program_options debug library
#  Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE   The Boost program_options release library
#
#  Boost_PYTHON_LIBRARY     The Boost python libraries
#  Boost_PYTHON_LIBRARY_DEBUG     The Boost python debug library
#  Boost_PYTHON_LIBRARY_RELEASE   The Boost python release library
#
#  Boost_REGEX_LIBRARY     The Boost regex libraries
#  Boost_REGEX_LIBRARY_DEBUG     The Boost regex debug library
#  Boost_REGEX_LIBRARY_RELEASE   The Boost regex release library
#
#  Boost_SERIALIZATION_LIBRARY     The Boost serialization libraries
#  Boost_SERIALIZATION_LIBRARY_DEBUG     The Boost serialization debug library
#  Boost_SERIALIZATION_LIBRARY_RELEASE   The Boost serialization release library
#
#  Boost_SIGNALS_LIBRARY     The Boost signals libraries
#  Boost_SIGNALS_LIBRARY_DEBUG     The Boost signals debug library
#  Boost_SIGNALS_LIBRARY_RELEASE   The Boost signals release library

#  Boost_SYSTEM_LIBRARY     The Boost system libraries
#  Boost_SYSTEM_LIBRARY_DEBUG     The Boost system debug library
#  Boost_SYSTEM_LIBRARY_RELEASE   The Boost system release library
#
#  Boost_TEST_EXEC_MONITOR_LIBRARY     The Boost test_exec_monitor libraries
#  Boost_TEST_EXEC_MONITOR_LIBRARY_DEBUG     The Boost test_exec_monitor debug library
#  Boost_TEST_EXEC_MONITOR_LIBRARY_RELEASE   The Boost test_exec_monitor release library
#
#  Boost_THREAD_LIBRARY     The Boost thread libraries
#  Boost_THREAD_LIBRARY_DEBUG     The Boost thread debug library
#  Boost_THREAD_LIBRARY_RELEASE   The Boost thread release library
#
#  Boost_UNIT_TEST_FRAMEWORK_LIBRARY     The Boost unit_test_framework libraries
#  Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG     The Boost unit_test_framework debug library
#  Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE   The Boost unit_test_framework release library
#
#  Boost_WSERIALIZATION_LIBRARY     The Boost wserialization libraries
#  Boost_WSERIALIZATION_LIBRARY_DEBUG     The Boost wserialization debug library
#  Boost_WSERIALIZATION_LIBRARY_RELEASE   The Boost wserialization release library
#
#  Boost_SYSTEM_LIBRARY           The Boost system libraries
#  Boost_SYSTEM_LIBRARY_DEBUG     The Boost system debug library
#  Boost_SYSTEM_LIBRARY_RELEASE   The Boost system release library
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#  Copyright (c) 2007 Wengo
#
#  Redistribution AND use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
OPTION (Boost_USE_MULTITHREADED "Use the Multithreaded boost libraries" ON)
MARK_AS_ADVANCED (Boost_USE_MULTITHREADED)
SET (Boost_DEBUG 0)

# List the versions of Boost that we are going to look for
#"1.36.1" "1.36.0" "1.35.1" "1.35.0" "1.35" "1.34.1" "1.34.0" "1.34" "1.33.1" "1.33.0" "1.33" )
SET( _boost_TEST_VERSIONS ${Boost_ADDITIONAL_VERSIONS} "1.38 1.37 1.36.1" "1.36" "1.35.1" "1.35.0" "1.35" "1.34.1" "1.34.0" "1.34" "1.33.1" "1.33.0" "1.33" )
SET (Boost_LIBRARIES "")
SET (Boost_INCLUDE_DIRS "")
IF (Boost_INCLUDE_DIRS)
  # in cache already
  SET(Boost_FOUND TRUE)
ELSE (Boost_INCLUDE_DIRS)

  # Add in some path suffixes. These will have to be updated whenever
  # a new Boost version comes out.
  #SET(Boost_PATH_SUFFIX
  #  boost-${Boost_CURRENT_VERSION}
  #)

  SET(Boost_INCLUDE_SEARCH_DIRS
    $ENV{BOOSTINCLUDEDIR}
    $ENV{BOOST_ROOT}/include
    C:/boost/include
    # D: is very often the cdrom drive, IF you don't have a
    # cdrom inserted it will popup a very annoying dialog
    #D:/boost/include
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
  )
  
  SET(Boost_LIBRARIES_SEARCH_DIRS
    $ENV{BOOSTLIBDIR}
    $ENV{BOOST_ROOT}/lib
    C:/boost/lib
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
  )
 # MESSAGE (STATUS "Boost_LIBRARIES_SEARCH_DIRS: ${Boost_LIBRARIES_SEARCH_DIRS}")
  IF (WIN32)
    # In windows, automatic linking is performed, so you do not have to specify the libraries.
    # If you are linking to a dynamic runtime, then you can choose to link to either a static or a
    # dynamic Boost library, the default is to do a static link.  You can alter this for a specific
    # library "whatever" by defining Boost_WHATEVER_DYN_LINK to force Boost library "whatever" to
    # be linked dynamically.  Alternatively you can force all Boost libraries to dynamic link by
    # defining Boost_ALL_DYN_LINK.

    # This feature can be disabled for Boost library "whatever" by defining Boost_WHATEVER_NO_LIB,
    # or for all of Boost by defining Boost_ALL_NO_LIB.

    # If you want to observe which libraries are being linked against then defining
    # Boost_LIB_DIAGNOSTIC will cause the auto-linking code to emit a #pragma MESSAGE each time
    # a library is selected for linking.
    SET(Boost_LIB_DIAGNOSTIC_DEFINITIONS "-DBoost_LIB_DIAGNOSTIC" "-DBoost_ALL_NO_LIB")
  ENDIF(WIN32)

  #Try first in our own include search paths (e.g. BOOST_ROOT)
  # This was taken from the CMake 2.6 FindBoost module
  FOREACH(_boost_VER ${_boost_TEST_VERSIONS})
    IF( NOT Boost_INCLUDE_DIR )
      # Add in a path suffix, based on the required version, ideally we could
      # read this from version.hpp, but for that to work we'd need to know the include
      # dir already
      SET(_boost_PATH_SUFFIX
        boost-${_boost_VER}
      )
      
      SET ( Boost_CURRENT_VERSION ${_boost_VER} )
     # message(STATUS "Boost_CURRENT_VERSION: ${Boost_CURRENT_VERSION}")
      IF(_boost_PATH_SUFFIX MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+")
          STRING(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1_\\2_\\3" _boost_PATH_SUFFIX ${_boost_PATH_SUFFIX})
          STRING(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1_\\2_\\3" Boost_CURRENT_VERSION ${Boost_CURRENT_VERSION})
      ELSEIF(_boost_PATH_SUFFIX MATCHES "[0-9]+\\.[0-9]+")
          STRING(REGEX REPLACE "([0-9]+)\\.([0-9]+)" "\\1_\\2" _boost_PATH_SUFFIX ${_boost_PATH_SUFFIX})
          STRING(REGEX REPLACE "([0-9]+)\\.([0-9]+)" "\\1_\\2" Boost_CURRENT_VERSION ${Boost_CURRENT_VERSION})
      ENDIF(_boost_PATH_SUFFIX MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+")
    
      #message(STATUS "_boost_PATH_SUFFIX: ${_boost_PATH_SUFFIX}")
      #message(STATUS "Boost_INCLUDE_SEARCH_DIRS: ${Boost_INCLUDE_SEARCH_DIRS}")
      FIND_PATH(Boost_INCLUDE_DIR
          NAMES         boost/config.hpp
          PATHS         ${Boost_INCLUDE_SEARCH_DIRS}
          PATH_SUFFIXES ${_boost_PATH_SUFFIX}
          NO_DEFAULT_PATH
      )

    ENDIF( NOT Boost_INCLUDE_DIR )
  ENDFOREACH(_boost_VER)

############################################
#
# Check the existence of the libraries.
#
############################################
# This macro was taken directly from the FindQt4.cmake file that is included
# with the CMake distribution. This is NOT my work. All work was done by the
# original authors of the FindQt4.cmake file. Only minor modifications were
# made to remove references to Qt and make this file more generally applicable
#########################################################################

MACRO (_Boost_ADJUST_LIB_VARS basename)
    
  IF (Boost_INCLUDE_DIR )
    #MESSAGE(STATUS "Adjusting ${basename} ")
      # if only the release version was found, set the debug variable also to the release version
      IF (Boost_${basename}_LIBRARY_RELEASE AND NOT Boost_${basename}_LIBRARY_DEBUG)
        SET(Boost_${basename}_LIBRARY_DEBUG ${Boost_${basename}_LIBRARY_RELEASE})
        SET(Boost_${basename}_LIBRARY       ${Boost_${basename}_LIBRARY_RELEASE})
        SET(Boost_${basename}_LIBRARIES     ${Boost_${basename}_LIBRARY_RELEASE})
      ENDIF (Boost_${basename}_LIBRARY_RELEASE AND NOT Boost_${basename}_LIBRARY_DEBUG)
    
      # if only the debug version was found, set the release variable also to the debug version
      IF (Boost_${basename}_LIBRARY_DEBUG AND NOT Boost_${basename}_LIBRARY_RELEASE)
        SET(Boost_${basename}_LIBRARY_RELEASE ${Boost_${basename}_LIBRARY_DEBUG})
        SET(Boost_${basename}_LIBRARY         ${Boost_${basename}_LIBRARY_DEBUG})
        SET(Boost_${basename}_LIBRARIES       ${Boost_${basename}_LIBRARY_DEBUG})
      ENDIF (Boost_${basename}_LIBRARY_DEBUG AND NOT Boost_${basename}_LIBRARY_RELEASE)
      IF (Boost_${basename}_LIBRARY_DEBUG AND Boost_${basename}_LIBRARY_RELEASE)
        # if the generator supports configuration types then set
        # optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
        IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
          SET(Boost_${basename}_LIBRARY       optimized ${Boost_${basename}_LIBRARY_RELEASE} debug ${Boost_${basename}_LIBRARY_DEBUG})
        ELSE(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
          # if there are no configuration types and CMAKE_BUILD_TYPE has no value
          # then just use the release libraries
          SET(Boost_${basename}_LIBRARY       ${Boost_${basename}_LIBRARY_RELEASE} )
        ENDIF(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        SET(Boost_${basename}_LIBRARIES       optimized ${Boost_${basename}_LIBRARY_RELEASE} debug ${Boost_${basename}_LIBRARY_DEBUG})
      ENDIF (Boost_${basename}_LIBRARY_DEBUG AND Boost_${basename}_LIBRARY_RELEASE)
    
      SET(Boost_${basename}_LIBRARY ${Boost_${basename}_LIBRARY} CACHE FILEPATH "The Boost ${basename} library")

      IF (Boost_${basename}_LIBRARY)
        SET(Boost_${basename}_FOUND 1)
        IF (Boost_USE_${basename})
            SET (Boost_LIBRARIES ${Boost_LIBRARIES} ${Boost_${basename}_LIBRARY} )
            #MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
        ENDIF(Boost_USE_${basename})
      ENDIF (Boost_${basename}_LIBRARY)

  ENDIF (Boost_INCLUDE_DIR )

  # Make variables changeble to the advanced user
  MARK_AS_ADVANCED(Boost_${basename}_LIBRARY 
                Boost_${basename}_LIBRARY_RELEASE 
                Boost_${basename}_LIBRARY_DEBUG 
                Boost_INCLUDE_DIR
                )
ENDMACRO (_Boost_ADJUST_LIB_VARS)

#-------------------------------------------------------------------------------
SET (Boost_LIB_PREFIX "")
IF ( MSVC )
    SET (Boost_LIB_PREFIX "lib")  # Looks for the static libraries by default
ENDIF ( MSVC )
  
IF (NOT DEFINED Boost_COMPILER)
    SET (Boost_COMPILER "-gcc")
    IF (MSVC71)
        SET (Boost_COMPILER "-vc71")
    ENDIF(MSVC71)
    IF (MSVC80)
        SET (Boost_COMPILER "-vc80")
    ENDIF(MSVC80)
    IF (MSVC90)
        SET (Boost_COMPILER "-vc90")
    ENDIF (MSVC90)
    IF (MINGW)
        EXEC_PROGRAM(${CMAKE_CXX_COMPILER}
            ARGS -dumpversion
            OUTPUT_VARIABLE _boost_COMPILER_VERSION
        )
        STRING(REGEX REPLACE ".* ([0-9])\\.([0-9])\\.[0-9] .*" "\\1\\2"
           _boost_COMPILER_VERSION ${_boost_COMPILER_VERSION})
        SET (Boost_COMPILER "-mgw${_boost_COMPILER_VERSION}")
    ENDIF(MINGW)
    IF (CYGWIN)
        SET (Boost_COMPILER "-gcc")
    ENDIF (CYGWIN)
    IF (UNIX)
        IF (APPLE)
            SET (Boost_COMPILER "-xgcc40")
        ELSE (APPLE)
            IF (NOT CMAKE_COMPILER_IS_GNUCC)
                SET (Boost_COMPILER "-il")  # This is for the intel compiler
            ELSE (NOT CMAKE_COMPILER_IS_GNUCC)
                EXEC_PROGRAM(${CMAKE_CXX_COMPILER}
                    ARGS -dumpversion
                    OUTPUT_VARIABLE _boost_COMPILER_VERSION
                )
                STRING(REGEX REPLACE ".* ([0-9])\\.([0-9])\\.[0-9] .*" "\\1\\2"
                   _boost_COMPILER_VERSION ${_boost_COMPILER_VERSION})
                SET (Boost_COMPILER "-gcc${_boost_COMPILER_VERSION}")
            ENDIF (NOT CMAKE_COMPILER_IS_GNUCC)
        ENDIF (APPLE)
    ENDIF(UNIX)
  ENDIF (NOT DEFINED Boost_COMPILER)

  IF(Boost_USE_MULTITHREADED)
    SET (Boost_MULTITHREADED "-mt")
  ENDIF(Boost_USE_MULTITHREADED)
  
  IF (MSVC)
    SET (Boost_ABI_TAG "g")
  ENDIF(MSVC)
  SET (Boost_ABI_TAG "-${Boost_ABI_TAG}d")
  
  SET (Boost_EXTENSION "a")
  IF (MSVC)
    SET (Boost_EXTENSION "lib")
  ENDIF (MSVC)

# MESSAGE (STATUS "Boost_LIBRARIES_SEARCH_DIRS: ${Boost_LIBRARIES_SEARCH_DIRS}")

# ------------------------------------------------------------------------
#  Begin finding boost libraries
# ------------------------------------------------------------------------
MACRO (_FIND_Boost_LIBRARY basename libname)
  
   IF (Boost_USE_${basename})
    IF (Boost_DEBUG)
      MESSAGE (STATUS "Looking for ${basename}...")
    ENDIF (Boost_DEBUG)
# -------- Find the date_time Library ------------- 
   SET (Boost_LIB ${libname})
   SET (Boost_DEBUG_LIB_NAME boost_${Boost_LIB}${Boost_COMPILER}${Boost_MULTITHREADED}${Boost_ABI_TAG}-${Boost_CURRENT_VERSION})
   SET (Boost_RELEASE_LIB_NAME boost_${Boost_LIB}${Boost_COMPILER}${Boost_MULTITHREADED}-${Boost_CURRENT_VERSION})
   IF (Boost_DEBUG)
      message(STATUS "${basename} Boost_DEBUG_LIB_NAME: ${Boost_DEBUG_LIB_NAME}")
      message(STATUS "${basename} Boost_RELEASE_LIB_NAME: ${Boost_RELEASE_LIB_NAME}")
      message(STATUS "NAMES: ${Boost_LIB_PREFIX}${Boost_DEBUG_LIB_NAME}")
      message(STATUS "PATHS: ${Boost_LIBRARIES_SEARCH_DIRS}")
   ENDIF (Boost_DEBUG)
  #-- Find a Debug Library ---------------------------------------------
   FIND_LIBRARY(Boost_${basename}_LIBRARY_DEBUG
    NAMES ${Boost_LIB_PREFIX}${Boost_DEBUG_LIB_NAME}
    PATHS ${Boost_LIBRARIES_SEARCH_DIRS}
    NO_DEFAULT_PATH
   )
   IF (Boost_${basename}_LIBRARY_DEBUG)
    SET(Boost_DEBUG_LIBRARIES ${Boost_DEBUG_LIBRARIES} ${Boost_${basename}_LIBRARY_DEBUG} )
   ENDIF (Boost_${basename}_LIBRARY_DEBUG)
  #-- Find a Release Library -------------------------------------------
   FIND_LIBRARY(Boost_${basename}_LIBRARY_RELEASE
    NAMES ${Boost_LIB_PREFIX}${Boost_RELEASE_LIB_NAME}
    PATHS ${Boost_LIBRARIES_SEARCH_DIRS}
    NO_DEFAULT_PATH
   )
   IF (Boost_${basename}_LIBRARY_RELEASE)
    SET(Boost_RELEASE_LIBRARIES ${Boost_DEBUG_LIBRARIES} ${Boost_${basename}_LIBRARY_RELEASE} )
   ENDIF (Boost_${basename}_LIBRARY_RELEASE)
  # -- Adjust all the library variables --------------------------------
   _Boost_ADJUST_LIB_VARS(${basename})
   IF (Boost_DEBUG)
      message(STATUS "Boost_${basename}_LIBRARY_DEBUG: ${Boost_${basename}_LIBRARY_DEBUG}")
      message(STATUS "Boost_${basename}_LIBRARY_RELEASE: ${Boost_${basename}_LIBRARY_RELEASE}")
   ENDIF (Boost_DEBUG)
   
   ENDIF (Boost_USE_${basename})
ENDMACRO(_FIND_Boost_LIBRARY basename libname )

# -------- Find the date_time Library -------------
_FIND_Boost_LIBRARY(DATE_TIME date_time)

# -------- Find the filesystem Library ------------- 
_FIND_Boost_LIBRARY(FILESYSTEM filesystem)

# -------- Find the iostreams Library ------------- 
_FIND_Boost_LIBRARY(IOSTREAMS iostreams)

# -------- Find the prg_exec_monitor Library ------------- 
_FIND_Boost_LIBRARY(PRO_EXEC_MONITOR prg_exec_monitor)

# -------- Find the program_options Library ------------- 
_FIND_Boost_LIBRARY(PROGRAM_OPTIONS program_options)
 
# -------- Find the python Library ------------- 
_FIND_Boost_LIBRARY(PYTHON python)
 
# -------- Find the regex Library ------------- 
_FIND_Boost_LIBRARY(REGEX regex)

# -------- Find the serialization Library ------------- 
_FIND_Boost_LIBRARY(SERIALIZATION serialization)
 
# -------- Find the signals Library ------------- 
_FIND_Boost_LIBRARY(SIGNALS signals)

# -------- Find the test_exec_monitor Library -------------
_FIND_Boost_LIBRARY(TEST_EXEC_MONITOR test_exec_monitor)
 
# -------- Find the thread Library ------------- 
_FIND_Boost_LIBRARY(THREAD thread)
 
# -------- Find the unit_test_framework Library -------------
_FIND_Boost_LIBRARY(UNIT_TEST_FRAMEWORK unit_test_framework)
 
# -------- Find the wserialization Library -------------
_FIND_Boost_LIBRARY(WSERIALIZATION wserialization)
 
# -------- Find the system Library -------------
_FIND_Boost_LIBRARY(SYSTEM system)


# ------------------------------------------------------------------------
#  End finding boost libraries
# ------------------------------------------------------------------------

SET(Boost_INCLUDE_DIRS
  ${Boost_INCLUDE_DIR}
)

# MESSAGE(STATUS "Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
# MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

IF (Boost_INCLUDE_DIRS)
  SET(Boost_FOUND TRUE)
ENDIF(Boost_INCLUDE_DIRS)

IF (Boost_FOUND)
    IF (NOT Boost_FIND_QUIETLY)
      MESSAGE (STATUS "Found Boost Version ${Boost_CURRENT_VERSION}" )
      MESSAGE (STATUS "Found The Following Boost Libraries:")
      FOREACH (TMP_LIB  ${Boost_LIBRARIES} )
        MESSAGE (STATUS "  ${TMP_LIB}")
      ENDFOREACH(TMP_LIB)
    ENDIF(NOT Boost_FIND_QUIETLY)
ELSE (Boost_FOUND)
    IF (Boost_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Please install the Boost libraries AND development packages")
    ENDIF(Boost_FIND_REQUIRED)
ENDIF(Boost_FOUND)

FOREACH (Boost_LIBDIR ${Boost_LIBRARIES})
    GET_FILENAME_COMPONENT(Boost_LIBRARY_DIRS ${Boost_LIBDIR} PATH)
ENDFOREACH (Boost_LIBDIR ${Boost_LIBRARIES})

  # Under MSVC on Windows, automatic linking is performed, so no need to specify 
  # the libraries. MinGW/MSYS/Cygwin still need these defined
IF (MSVC)
    SET(Boost_LIBRARIES "")
ENDIF(MSVC)

  # show the Boost_INCLUDE_DIRS AND Boost_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(Boost_INCLUDE_DIRS 
    Boost_LIBRARIES 
    Boost_LIBRARY_DIRS 
    Boost_DEFINITIONS 
    Boost_LIBRARIES_SUFFIX
)

ENDIF(Boost_INCLUDE_DIRS)
