

# In this file we are doing all of our 'configure' checks. Things like checking
# for headers, functions, libraries, types and size of types.
INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckCXXSourceCompiles.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake)


TEST_BIG_ENDIAN(CMP_WORDS_BIGENDIAN)

# --------------------------------------------------------------------
#  Now check for needed Header files
# --------------------------------------------------------------------
macro (CORE_CHECK_INCLUDE_FILE header var prefix)
    CHECK_INCLUDE_FILE("${header}"        ${prefix}_${var} )
endmacro()


CORE_CHECK_INCLUDE_FILE("stddef.h"        HAVE_STDDEF_H   CMP)
CORE_CHECK_INCLUDE_FILE("stdint.h"        HAVE_STDINT_H   CMP)
CORE_CHECK_INCLUDE_FILE("stdlib.h"        HAVE_STDLIB_H   CMP)
CORE_CHECK_INCLUDE_FILE("setjmp.h"        HAVE_SETJMP_H   CMP)
CORE_CHECK_INCLUDE_FILE("string.h"        HAVE_STRING_H   CMP)
CORE_CHECK_INCLUDE_FILE("stdio.h"         HAVE_STDIO_H   CMP)
CORE_CHECK_INCLUDE_FILE("math.h"          HAVE_MATH_H   CMP)
CORE_CHECK_INCLUDE_FILE("time.h"          HAVE_TIME_H   CMP)
CORE_CHECK_INCLUDE_FILE("sys/time.h"      HAVE_SYS_TIME_H   CMP)
CORE_CHECK_INCLUDE_FILE("sys/types.h"     HAVE_SYS_TYPES_H   CMP)
CORE_CHECK_INCLUDE_FILE("sys/socket.h"    HAVE_SYS_SOCKET_H   CMP)
CORE_CHECK_INCLUDE_FILE("sys/stat.h"      HAVE_SYS_STAT_H   CMP)
CORE_CHECK_INCLUDE_FILE("netinet/in.h"    HAVE_NETINET_IN_H   CMP)
CORE_CHECK_INCLUDE_FILE("arpa/inet.h"     HAVE_ARPA_INET_H   CMP)
CORE_CHECK_INCLUDE_FILE("unistd.h"        HAVE_UNISTD_H   CMP)
CORE_CHECK_INCLUDE_FILE("fcntl.h"         HAVE_FCNTL_H   CMP)
CORE_CHECK_INCLUDE_FILE("errno.h"         HAVE_ERRNO_H   CMP)

if(WIN32)
  if(NOT UNIX)
    SET(CMP_HAVE_WINDOWS 1)
    SET(CMP_HAVE_WINSOCK_H 1)
  ENDif(NOT UNIX)
ENDif(WIN32)

#-----------------------------------------------------------------------------
#  Check the size in bytes of all the int and float types
#-----------------------------------------------------------------------------
MACRO ( CORE_CHECK_TYPE_SIZE type var)
    set(aType ${type})
    set(aVar  ${var})
    CHECK_TYPE_SIZE(${aType}   ${aVar})
    if(NOT ${aVar})
        set(${aVar} 0 CACHE INTERNAL "SizeOf for ${aType}")
    ENDif()
ENDMACRO()

CORE_CHECK_TYPE_SIZE(char           CMP_SIZEOF_CHAR)
CORE_CHECK_TYPE_SIZE(short          CMP_SIZEOF_SHORT)
CORE_CHECK_TYPE_SIZE(int            CMP_SIZEOF_INT)
CORE_CHECK_TYPE_SIZE(unsigned       CMP_SIZEOF_UNSIGNED)
if(NOT APPLE)
    CORE_CHECK_TYPE_SIZE(long           CMP_SIZEOF_LONG)
ENDif()
CORE_CHECK_TYPE_SIZE("long long"    CMP_SIZEOF_LONG_LONG)
CORE_CHECK_TYPE_SIZE(__int64        CMP_SIZEOF___INT64)
if(NOT CMP_SIZEOF___INT64)
    set(CMP_SIZEOF___INT64 0)
ENDif()

CORE_CHECK_TYPE_SIZE(float          CMP_SIZEOF_FLOAT)
CORE_CHECK_TYPE_SIZE(double         CMP_SIZEOF_DOUBLE)
CORE_CHECK_TYPE_SIZE("long double"  CMP_SIZEOF_LONG_DOUBLE)
CORE_CHECK_TYPE_SIZE(int8_t         CMP_SIZEOF_INT8_T)
CORE_CHECK_TYPE_SIZE(uint8_t        CMP_SIZEOF_UINT8_T)
#CORE_CHECK_TYPE_SIZE(int_least8_t   CMP_SIZEOF_INT_LEAST8_T)
#CORE_CHECK_TYPE_SIZE(uint_least8_t  CMP_SIZEOF_UINT_LEAST8_T)
#CORE_CHECK_TYPE_SIZE(int_fast8_t    CMP_SIZEOF_INT_FAST8_T)
#CORE_CHECK_TYPE_SIZE(uint_fast8_t   CMP_SIZEOF_UINT_FAST8_T)
CORE_CHECK_TYPE_SIZE(int16_t        CMP_SIZEOF_INT16_T)
CORE_CHECK_TYPE_SIZE(uint16_t       CMP_SIZEOF_UINT16_T)
#CORE_CHECK_TYPE_SIZE(int_least16_t  CMP_SIZEOF_INT_LEAST16_T)
#CORE_CHECK_TYPE_SIZE(uint_least16_t CMP_SIZEOF_UINT_LEAST16_T)
#CORE_CHECK_TYPE_SIZE(int_fast16_t   CMP_SIZEOF_INT_FAST16_T)
#CORE_CHECK_TYPE_SIZE(uint_fast16_t  CMP_SIZEOF_UINT_FAST16_T)
CORE_CHECK_TYPE_SIZE(int32_t        CMP_SIZEOF_INT32_T)
CORE_CHECK_TYPE_SIZE(uint32_t       CMP_SIZEOF_UINT32_T)
#CORE_CHECK_TYPE_SIZE(int_least32_t  CMP_SIZEOF_INT_LEAST32_T)
#CORE_CHECK_TYPE_SIZE(uint_least32_t CMP_SIZEOF_UINT_LEAST32_T)
#CORE_CHECK_TYPE_SIZE(int_fast32_t   CMP_SIZEOF_INT_FAST32_T)
#CORE_CHECK_TYPE_SIZE(uint_fast32_t  CMP_SIZEOF_UINT_FAST32_T)
CORE_CHECK_TYPE_SIZE(int64_t        CMP_SIZEOF_INT64_T)
CORE_CHECK_TYPE_SIZE(uint64_t       CMP_SIZEOF_UINT64_T)
#CORE_CHECK_TYPE_SIZE(int_least64_t  CMP_SIZEOF_INT_LEAST64_T)
#CORE_CHECK_TYPE_SIZE(uint_least64_t CMP_SIZEOF_UINT_LEAST64_T)
#CORE_CHECK_TYPE_SIZE(int_fast64_t   CMP_SIZEOF_INT_FAST64_T)
#CORE_CHECK_TYPE_SIZE(uint_fast64_t  CMP_SIZEOF_UINT_FAST64_T)
if(NOT APPLE)
    CORE_CHECK_TYPE_SIZE(size_t         CMP_SIZEOF_SIZE_T)
    CORE_CHECK_TYPE_SIZE(ssize_t        CMP_SIZEOF_SSIZE_T)
    if(NOT CMP_SIZEOF_SSIZE_T)
        set(CMP_SIZEOF_SSIZE_T 0)
    ENDif()
ENDif()
CORE_CHECK_TYPE_SIZE(off_t          CMP_SIZEOF_OFF_T)
CORE_CHECK_TYPE_SIZE(off64_t        CMP_SIZEOF_OFF64_T)
if(NOT CMP_SIZEOF_OFF64_T)
    set(CMP_SIZEOF_OFF64_T 0)
ENDif()


if(CMP_SIZEOF___INT64)
  if(CMP_TYPE_SAME_LONG_AND___INT64 MATCHES "^CMP_TYPE_SAME_LONG_AND___INT64$")
    MESSAGE(STATUS "Checking whether long and __int64 are the same type")
    TRY_COMPILE(CMP_TYPE_SAME_LONG_AND___INT64
      ${PROJECT_BINARY_DIR}/CMakeTmp
      ${CMP_CORE_TESTS_SOURCE_DIR}/TestCompareTypes.cxx
      COMPILE_DEFINITIONS
      -DCMP_TEST_COMPARE_TYPE_1=long
      -DCMP_TEST_COMPARE_TYPE_2=__int64
      OUTPUT_VARIABLE OUTPUT)
    if(CMP_TYPE_SAME_LONG_AND___INT64)
      MESSAGE(STATUS "Checking whether long and __int64 are the same type -- yes")
      SET(CMP_TYPE_SAME_LONG_AND___INT64 1 CACHE INTERNAL "Whether long and __int64 are the same type")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
        "Determining whether long and __int64 are the same type "
        "passed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ELSE(CMP_TYPE_SAME_LONG_AND___INT64)
      MESSAGE(STATUS "Checking whether long and __int64 are the same type -- no")
      SET(CMP_TYPE_SAME_LONG_AND___INT64 0 CACHE INTERNAL "Whether long and __int64 are the same type")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
        "Determining whether long and __int64 are the same type "
        "failed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ENDif(CMP_TYPE_SAME_LONG_AND___INT64)
  ENDif(CMP_TYPE_SAME_LONG_AND___INT64 MATCHES "^CMP_TYPE_SAME_LONG_AND___INT64$")
  if(CMP_SIZEOF_LONG_LONG)
    if(CMP_TYPE_SAME_LONG_LONG_AND___INT64 MATCHES "^CMP_TYPE_SAME_LONG_LONG_AND___INT64$")
      MESSAGE(STATUS "Checking whether long long and __int64 are the same type")
      TRY_COMPILE(CMP_TYPE_SAME_LONG_LONG_AND___INT64
        ${PROJECT_BINARY_DIR}/CMakeTmp
        ${CMP_CORE_TESTS_SOURCE_DIR}/TestCompareTypes.cxx
        COMPILE_DEFINITIONS
        -DCMP_TEST_COMPARE_TYPE_1=TYPE_LONG_LONG
        -DCMP_TEST_COMPARE_TYPE_2=__int64
        OUTPUT_VARIABLE OUTPUT)
      if(CMP_TYPE_SAME_LONG_LONG_AND___INT64)
        MESSAGE(STATUS "Checking whether long long and __int64 are the same type -- yes")
        SET(CMP_TYPE_SAME_LONG_LONG_AND___INT64 1 CACHE INTERNAL "Whether long long and __int64 are the same type")
        WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
          "Determining whether long long and __int64 are the same type "
          "passed with the following output:\n"
          "${OUTPUT}\n" APPEND)
      ELSE(CMP_TYPE_SAME_LONG_LONG_AND___INT64)
        MESSAGE(STATUS "Checking whether long long and __int64 are the same type -- no")
        SET(CMP_TYPE_SAME_LONG_LONG_AND___INT64 0 CACHE INTERNAL "Whether long long and __int64 are the same type")
        WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
          "Determining whether long long and __int64 are the same type "
          "failed with the following output:\n"
          "${OUTPUT}\n" APPEND)
      ENDif(CMP_TYPE_SAME_LONG_LONG_AND___INT64)
    ENDif(CMP_TYPE_SAME_LONG_LONG_AND___INT64 MATCHES "^CMP_TYPE_SAME_LONG_LONG_AND___INT64$")
  ENDif(CMP_SIZEOF_LONG_LONG)
ENDif(CMP_SIZEOF___INT64)

# Enable the "long long" type if it is available.  It is standard in
# C99 and C++03 but not in earlier standards.
SET(CMP_TYPE_USE_LONG_LONG)
if(CMP_SIZEOF_LONG_LONG)
  SET(CMP_TYPE_USE_LONG_LONG 1)
ENDif(CMP_SIZEOF_LONG_LONG)

# Enable the "__int64" type if it is available and unique.  It is not
# standard.
SET(CMP_TYPE_USE___INT64)
if(CMP_SIZEOF___INT64)
  if(NOT CMP_TYPE_SAME_LONG_AND___INT64)
    if(NOT CMP_TYPE_SAME_LONG_LONG_AND___INT64)
      SET(CMP_TYPE_USE___INT64 1)
    ENDif(NOT CMP_TYPE_SAME_LONG_LONG_AND___INT64)
  ENDif(NOT CMP_TYPE_SAME_LONG_AND___INT64)
ENDif(CMP_SIZEOF___INT64)

if(CMP_COMPILER_HAS_BOOL MATCHES "^CMP_COMPILER_HAS_BOOL$")
  #MESSAGE(STATUS "Checking support for C++ type bool")
  TRY_COMPILE(CMP_COMPILER_HAS_BOOL
              ${PROJECT_BINARY_DIR}/CMakeTmp/Bool
              ${CMP_CORE_TESTS_SOURCE_DIR}/TestBoolType.cxx
              OUTPUT_VARIABLE OUTPUT)
  if(CMP_COMPILER_HAS_BOOL)
    MESSAGE(STATUS "Checking support for C++ type bool -- yes")
    SET(CMP_COMPILER_HAS_BOOL 1 CACHE INTERNAL "Support for C++ type bool")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
      "Determining if the C++ compiler supports type bool "
      "passed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ELSE(CMP_COMPILER_HAS_BOOL)
    MESSAGE(STATUS "Checking support for C++ type bool -- no")
    SET(CMP_COMPILER_HAS_BOOL 0 CACHE INTERNAL "Support for C++ type bool")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
      "Determining if the C++ compiler supports type bool "
      "failed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ENDif(CMP_COMPILER_HAS_BOOL)
ENDif()

if(CMP_TYPE_CHAR_IS_SIGNED MATCHES "^CMP_TYPE_CHAR_IS_SIGNED$")
  #MESSAGE(STATUS "Checking signedness of char")
  TRY_RUN(CMP_TYPE_CHAR_IS_SIGNED CMP_TYPE_CHAR_IS_SIGNED_COMPILED
          ${PROJECT_BINARY_DIR}/CMakeTmp/Char
          ${CMP_CORE_TESTS_SOURCE_DIR}/TestCharSignedness.cxx)
  if(CMP_TYPE_CHAR_IS_SIGNED_COMPILED)
    if(CMP_TYPE_CHAR_IS_SIGNED)
      MESSAGE(STATUS "Checking signedness of char -- signed")
      SET(CMP_TYPE_CHAR_IS_SIGNED 1 CACHE INTERNAL "Whether char is signed.")
    ELSE(CMP_TYPE_CHAR_IS_SIGNED)
      MESSAGE(STATUS "Checking signedness of char -- unsigned")
      SET(CMP_TYPE_CHAR_IS_SIGNED 0 CACHE INTERNAL "Whether char is signed.")
    ENDif(CMP_TYPE_CHAR_IS_SIGNED)
  ELSE(CMP_TYPE_CHAR_IS_SIGNED_COMPILED)
    MESSAGE(STATUS "Checking signedness of char -- failed")
  ENDif(CMP_TYPE_CHAR_IS_SIGNED_COMPILED)
ENDif()


set(VERSION_COMPILE_FLAGS "")

if(MSVC)
 set(CMP_HAVE_TIME_GETTIMEOFDAY "TRUE")
 set(VERSION_COMPILE_FLAGS "/DCMP_HAVE_TIME_GETTIMEOFDAY")
endif()

if(NOT MSVC)
    TRY_COMPILE(CMP_HAVE_TIME_GETTIMEOFDAY
          ${CMAKE_BINARY_DIR}
          ${CMP_CORE_TESTS_SOURCE_DIR}/GetTimeOfDayTest.cpp
          COMPILE_DEFINITIONS -DTRY_TIME_H
          OUTPUT_VARIABLE OUTPUT)
    if(CMP_HAVE_TIME_GETTIMEOFDAY STREQUAL "TRUE")
        set(CMP_HAVE_TIME_GETTIMEOFDAY "1")
        set(VERSION_COMPILE_FLAGS "-DCMP_HAVE_TIME_GETTIMEOFDAY")
    endif(CMP_HAVE_TIME_GETTIMEOFDAY STREQUAL "TRUE")

    TRY_COMPILE(CMP_HAVE_SYS_TIME_GETTIMEOFDAY
          ${CMAKE_BINARY_DIR}
          ${CMP_CORE_TESTS_SOURCE_DIR}/GetTimeOfDayTest.cpp
          COMPILE_DEFINITIONS -DTRY_SYS_TIME_H
          OUTPUT_VARIABLE OUTPUT)
    if(CMP_HAVE_SYS_TIME_GETTIMEOFDAY STREQUAL "TRUE")
        set(CMP_HAVE_SYS_TIME_GETTIMEOFDAY "1")
        set(VERSION_COMPILE_FLAGS "-DCMP_HAVE_SYS_TIME_GETTIMEOFDAY")
    endif(CMP_HAVE_SYS_TIME_GETTIMEOFDAY STREQUAL "TRUE")

    if(NOT CMP_HAVE_SYS_TIME_GETTIMEOFDAY AND NOT CMP_HAVE_TIME_GETTIMEOFDAY)
        message(STATUS "---------------------------------------------------------------")
        message(STATUS "Function 'gettimeofday()' was not found. CMPDataModel will use its")
        message(STATUS "  own implementation.. This can happen on older versions of")
        message(STATUS "  MinGW on Windows. Consider upgrading your MinGW installation")
        message(STATUS "  to a newer version such as MinGW 3.12")
        message(STATUS "---------------------------------------------------------------")
    endif()
endif(NOT MSVC)

#-----------------------------------------------------------------------------
# Check how to print a Long Long integer
#-----------------------------------------------------------------------------
SET(CMP_CMP_PRINTF_LL_WIDTH "CMP_PRINTF_LL_WIDTH")
if(CMP_PRINTF_LL_WIDTH MATCHES "^CMP_PRINTF_LL_WIDTH$")
  SET(PRINT_LL_FOUND 0)
  MESSAGE(STATUS "Checking for appropriate format for 64 bit long:")
  FOREACH(CMP_PRINTF_LL l64 l L q I64 ll)
    SET(CURRENT_TEST_DEFINITIONS "-DPRINTF_LL_WIDTH=${CMP_PRINTF_LL}")
    if(CMP_SIZEOF_LONG_LONG)
      SET(CURRENT_TEST_DEFINITIONS "${CURRENT_TEST_DEFINITIONS} -DHAVE_LONG_LONG")
    endif(CMP_SIZEOF_LONG_LONG)
    TRY_RUN(CMP_PRINTF_LL_TEST_RUN   CMP_PRINTF_LL_TEST_COMPILE
      ${CMAKE_BINARY_DIR}/CMake
      ${CMP_CORE_TESTS_SOURCE_DIR}/TestMiscFeatures.c
      CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${CURRENT_TEST_DEFINITIONS}
      OUTPUT_VARIABLE OUTPUT)
    if(CMP_PRINTF_LL_TEST_COMPILE)
      if(CMP_PRINTF_LL_TEST_RUN MATCHES 0)
        SET(CMP_PRINTF_LL_WIDTH "\"${CMP_PRINTF_LL}\"" CACHE INTERNAL "Width for printf for type `long long' or `__int64', us. `ll")
        SET(PRINT_LL_FOUND 1)
      ELSE (CMP_PRINTF_LL_TEST_RUN MATCHES 0)
        MESSAGE("Width with ${CMP_PRINTF_LL} failed with result: ${CMP_PRINTF_LL_TEST_RUN}")
      endif(CMP_PRINTF_LL_TEST_RUN MATCHES 0)
    ELSE (CMP_PRINTF_LL_TEST_COMPILE)
      FILE( APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
        "Test CMP_PRINTF_LL_WIDTH for ${CMP_PRINTF_LL} failed with the following output:\n ${OUTPUT}\n")
    endif(CMP_PRINTF_LL_TEST_COMPILE)
  ENDFOREACH(CMP_PRINTF_LL)
  if(PRINT_LL_FOUND)
    MESSAGE(STATUS "Checking for apropriate format for 64 bit long: found ${CMP_PRINTF_LL_WIDTH}")
  ELSE (PRINT_LL_FOUND)
    MESSAGE(STATUS "Checking for apropriate format for 64 bit long: not found")
    SET(CMP_PRINTF_LL_WIDTH "\"unknown\"" CACHE INTERNAL
      "Width for printf for type `long long' or `__int64', us. `ll")
  endif(PRINT_LL_FOUND)
endif(CMP_PRINTF_LL_WIDTH MATCHES "^CMP_PRINTF_LL_WIDTH$")

