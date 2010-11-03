INCLUDE(${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)
CHECK_TYPE_SIZE(char MXA_SIZEOF_CHAR)
CHECK_TYPE_SIZE(double MXA_SIZEOF_DOUBLE)
CHECK_TYPE_SIZE(float MXA_SIZEOF_FLOAT)
CHECK_TYPE_SIZE(int MXA_SIZEOF_INT)
CHECK_TYPE_SIZE(long MXA_SIZEOF_LONG)
CHECK_TYPE_SIZE(short MXA_SIZEOF_SHORT)

CHECK_TYPE_SIZE("long long" MXA_SIZEOF_LONG_LONG)
CHECK_TYPE_SIZE("__int64"   MXA_SIZEOF___INT64)

CHECK_TYPE_SIZE(size_t         MXA_SIZEOF_SIZE_T)
CHECK_TYPE_SIZE(ssize_t        MXA_SIZEOF_SSIZE_T)
IF (NOT MXA_SIZEOF_SSIZE_T )
  SET (MXA_SIZEOF_SSIZE_T 4)
ENDIF(NOT MXA_SIZEOF_SSIZE_T)

IF(MXA_SIZEOF___INT64)
  IF("MXA_TYPE_SAME_LONG_AND___INT64" MATCHES "^MXA_TYPE_SAME_LONG_AND___INT64$")
    MESSAGE(STATUS "Checking whether long and __int64 are the same type")
    TRY_COMPILE(MXA_TYPE_SAME_LONG_AND___INT64
      ${MXA_BINARY_DIR}/CMakeTmp
      ${AIMRepresentation_CMAKE_DIR}/vtkTestCompareTypes.cxx
      COMPILE_DEFINITIONS
      -DMXA_TEST_COMPARE_TYPE_1=long
      -DMXA_TEST_COMPARE_TYPE_2=__int64
      OUTPUT_VARIABLE OUTPUT)
    IF(MXA_TYPE_SAME_LONG_AND___INT64)
      MESSAGE(STATUS "Checking whether long and __int64 are the same type -- yes")
      SET(MXA_TYPE_SAME_LONG_AND___INT64 1 CACHE INTERNAL "Whether long and __int64 are the same type")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
        "Determining whether long and __int64 are the same type "
        "passed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ELSE(MXA_TYPE_SAME_LONG_AND___INT64)
      MESSAGE(STATUS "Checking whether long and __int64 are the same type -- no")
      SET(MXA_TYPE_SAME_LONG_AND___INT64 0 CACHE INTERNAL "Whether long and __int64 are the same type")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
        "Determining whether long and __int64 are the same type "
        "failed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ENDIF(MXA_TYPE_SAME_LONG_AND___INT64)
  ENDIF("MXA_TYPE_SAME_LONG_AND___INT64" MATCHES "^MXA_TYPE_SAME_LONG_AND___INT64$")
  IF(MXA_SIZEOF_LONG_LONG)
    IF("MXA_TYPE_SAME_LONG_LONG_AND___INT64" MATCHES "^MXA_TYPE_SAME_LONG_LONG_AND___INT64$")
      MESSAGE(STATUS "Checking whether long long and __int64 are the same type")
      TRY_COMPILE(MXA_TYPE_SAME_LONG_LONG_AND___INT64
        ${MXA_BINARY_DIR}/CMakeTmp
        ${AIMRepresentation_CMAKE_DIR}/vtkTestCompareTypes.cxx
        COMPILE_DEFINITIONS
        -DMXA_TEST_COMPARE_TYPE_1=TYPE_LONG_LONG
        -DMXA_TEST_COMPARE_TYPE_2=__int64
        OUTPUT_VARIABLE OUTPUT)
      IF(MXA_TYPE_SAME_LONG_LONG_AND___INT64)
        MESSAGE(STATUS "Checking whether long long and __int64 are the same type -- yes")
        SET(MXA_TYPE_SAME_LONG_LONG_AND___INT64 1 CACHE INTERNAL "Whether long long and __int64 are the same type")
        WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
          "Determining whether long long and __int64 are the same type "
          "passed with the following output:\n"
          "${OUTPUT}\n" APPEND)
      ELSE(MXA_TYPE_SAME_LONG_LONG_AND___INT64)
        MESSAGE(STATUS "Checking whether long long and __int64 are the same type -- no")
        SET(MXA_TYPE_SAME_LONG_LONG_AND___INT64 0 CACHE INTERNAL "Whether long long and __int64 are the same type")
        WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
          "Determining whether long long and __int64 are the same type "
          "failed with the following output:\n"
          "${OUTPUT}\n" APPEND)
      ENDIF(MXA_TYPE_SAME_LONG_LONG_AND___INT64)
    ENDIF("MXA_TYPE_SAME_LONG_LONG_AND___INT64" MATCHES "^MXA_TYPE_SAME_LONG_LONG_AND___INT64$")
  ENDIF(MXA_SIZEOF_LONG_LONG)
  IF(NOT MXA_TYPE_SAME_LONG_AND___INT64)
    IF(NOT MXA_TYPE_SAME_LONG_LONG_AND___INT64)
#  VS 6 cannot convert unsigned __int64 to double unless the
# "Visual C++ Processor Pack" is installed.
      IF("MXA_TYPE_CONVERT_UI64_TO_DOUBLE" MATCHES "^MXA_TYPE_CONVERT_UI64_TO_DOUBLE$")
        MESSAGE(STATUS "Checking whether unsigned __int64 can convert to double")
        TRY_COMPILE(MXA_TYPE_CONVERT_UI64_TO_DOUBLE
          ${MXA_BINARY_DIR}/CMakeTmp
          ${AIMRepresentation_CMAKE_DIR}/vtkTestConvertTypes.cxx
          COMPILE_DEFINITIONS
          -DMXA_TEST_CONVERT_TYPE_FROM=TYPE_UNSIGNED___INT64
          -DMXA_TEST_CONVERT_TYPE_TO=double
          OUTPUT_VARIABLE OUTPUT)
        IF(MXA_TYPE_CONVERT_UI64_TO_DOUBLE)
          MESSAGE(STATUS "Checking whether unsigned __int64 can convert to double -- yes")
          SET(MXA_TYPE_CONVERT_UI64_TO_DOUBLE 1 CACHE INTERNAL "Whether unsigned __int64 can convert to double")
          WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
            "Determining whether unsigned __int64 can convert to double "
            "passed with the following output:\n"
            "${OUTPUT}\n" APPEND)
        ELSE(MXA_TYPE_CONVERT_UI64_TO_DOUBLE)
          MESSAGE(STATUS "Checking whether unsigned __int64 can convert to double -- no")
          SET(MXA_TYPE_CONVERT_UI64_TO_DOUBLE 0 CACHE INTERNAL "Whether unsigned __int64 can convert to double")
          WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
            "Determining whether unsigned __int64 can convert to double "
            "failed with the following output:\n"
            "${OUTPUT}\n" APPEND)
        ENDIF(MXA_TYPE_CONVERT_UI64_TO_DOUBLE)
      ENDIF("MXA_TYPE_CONVERT_UI64_TO_DOUBLE" MATCHES "^MXA_TYPE_CONVERT_UI64_TO_DOUBLE$")
    ENDIF(NOT MXA_TYPE_SAME_LONG_LONG_AND___INT64)
  ENDIF(NOT MXA_TYPE_SAME_LONG_AND___INT64)
ENDIF(MXA_SIZEOF___INT64)

# Enable the "long long" type if it is available.  It is standard in
# C99 and C++03 but not in earlier standards.
SET(MXA_TYPE_USE_LONG_LONG)
IF(MXA_SIZEOF_LONG_LONG)
  SET(MXA_TYPE_USE_LONG_LONG 1)
ENDIF(MXA_SIZEOF_LONG_LONG)

# Enable the "__int64" type if it is available and unique.  It is not
# standard.
SET(MXA_TYPE_USE___INT64)
IF(MXA_SIZEOF___INT64)
  IF(NOT MXA_TYPE_SAME_LONG_AND___INT64)
    IF(NOT MXA_TYPE_SAME_LONG_LONG_AND___INT64)
      SET(MXA_TYPE_USE___INT64 1)
    ENDIF(NOT MXA_TYPE_SAME_LONG_LONG_AND___INT64)
  ENDIF(NOT MXA_TYPE_SAME_LONG_AND___INT64)
ENDIF(MXA_SIZEOF___INT64)

IF("MXA_COMPILER_HAS_BOOL" MATCHES "^MXA_COMPILER_HAS_BOOL$")
  MESSAGE(STATUS "Checking support for C++ type bool")
  TRY_COMPILE(MXA_COMPILER_HAS_BOOL
              ${PROJECT_BINARY_DIR}/CMakeTmp/Bool
              ${AIMRepresentation_CMAKE_DIR}/vtkTestBoolType.cxx
              OUTPUT_VARIABLE OUTPUT)
  IF(MXA_COMPILER_HAS_BOOL)
    MESSAGE(STATUS "Checking support for C++ type bool -- yes")
    SET(MXA_COMPILER_HAS_BOOL 1 CACHE INTERNAL "Support for C++ type bool")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
      "Determining if the C++ compiler supports type bool "
      "passed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ELSE(MXA_COMPILER_HAS_BOOL)
    MESSAGE(STATUS "Checking support for C++ type bool -- no")
    SET(MXA_COMPILER_HAS_BOOL 0 CACHE INTERNAL "Support for C++ type bool")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
      "Determining if the C++ compiler supports type bool "
      "failed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ENDIF(MXA_COMPILER_HAS_BOOL)
ENDIF("MXA_COMPILER_HAS_BOOL" MATCHES "^MXA_COMPILER_HAS_BOOL$")

IF("MXA_TYPE_CHAR_IS_SIGNED" MATCHES "^MXA_TYPE_CHAR_IS_SIGNED$")
  MESSAGE(STATUS "Checking signedness of char")
  TRY_RUN(MXA_TYPE_CHAR_IS_SIGNED MXA_TYPE_CHAR_IS_SIGNED_COMPILED
          ${PROJECT_BINARY_DIR}/CMakeTmp/Char
          ${AIMRepresentation_CMAKE_DIR}/vtkTestCharSignedness.cxx)
  IF(MXA_TYPE_CHAR_IS_SIGNED_COMPILED)
    IF(MXA_TYPE_CHAR_IS_SIGNED)
      MESSAGE(STATUS "Checking signedness of char -- signed")
      SET(MXA_TYPE_CHAR_IS_SIGNED 1 CACHE INTERNAL "Whether char is signed.")
    ELSE(MXA_TYPE_CHAR_IS_SIGNED)
      MESSAGE(STATUS "Checking signedness of char -- unsigned")
      SET(MXA_TYPE_CHAR_IS_SIGNED 0 CACHE INTERNAL "Whether char is signed.")
    ENDIF(MXA_TYPE_CHAR_IS_SIGNED)
  ELSE(MXA_TYPE_CHAR_IS_SIGNED_COMPILED)
    MESSAGE(STATUS "Checking signedness of char -- failed")
  ENDIF(MXA_TYPE_CHAR_IS_SIGNED_COMPILED)
ENDIF("MXA_TYPE_CHAR_IS_SIGNED" MATCHES "^MXA_TYPE_CHAR_IS_SIGNED$")

IF (VERBOSE)
MESSAGE (STATUS "MXA_SIZEOF_CHAR   = ${MXA_SIZEOF_CHAR} ")
MESSAGE (STATUS "MXA_SIZEOF_DOUBLE = ${MXA_SIZEOF_DOUBLE} ")
MESSAGE (STATUS "MXA_SIZEOF_FLOAT  = ${MXA_SIZEOF_FLOAT} ")
MESSAGE (STATUS "MXA_SIZEOF_INT    = ${MXA_SIZEOF_INT} ")
MESSAGE (STATUS "MXA_SIZEOF_LONG   = ${MXA_SIZEOF_LONG} ")
MESSAGE (STATUS "MXA_SIZEOF_SHORT  = ${MXA_SIZEOF_SHORT} ")
MESSAGE (STATUS "MXA_SIZEOF_LONG_LONG  = ${MXA_SIZEOF_LONG_LONG} ")
MESSAGE (STATUS "MXA_SIZEOF___INT64  = ${MXA_SIZEOF___INT64} ")
MESSAGE (STATUS "MXA_TYPE_SAME_LONG_AND___INT64 = ${MXA_TYPE_SAME_LONG_AND___INT64} ")
MESSAGE (STATUS "MXA_TYPE_USE_LONG_LONG = ${MXA_TYPE_USE_LONG_LONG} ")
MESSAGE (STATUS "MXA_COMPILER_HAS_BOOL = ${MXA_COMPILER_HAS_BOOL} ")
MESSAGE (STATUS "MXA_TYPE_CHAR_IS_SIGNED = ${MXA_TYPE_CHAR_IS_SIGNED} ")
ENDIF (VERBOSE)
