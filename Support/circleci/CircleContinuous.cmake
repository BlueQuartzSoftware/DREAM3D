set(CTEST_SOURCE_DIRECTORY "/dream3d")
set(CTEST_BINARY_DIRECTORY "/dream3d-build")
# We need to set this otherwise we yet 255 as our return code!
set(CTEST_COMMAND ctest)
include(${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake)
set(CTEST_SITE "CircleCI")
set(CTEST_BUILD_NAME "$ENV{CIRCLE_BRANCH}-#$ENV{CIRCLE_BUILD_NUM}")
set(CTEST_CMAKE_GENERATOR "Ninja")
set(CMAKE_BUILD_TYPE "Release")
set(CTEST_CONFIGURATION_TYPE "${CMAKE_BUILD_TYPE}")

#--------------------------------------------------------------------------------------------------
# Start the DREAM.3D Configuration Specific Settings
set(DREAM3D_SDK="/opt/DREAM3D_SDK")
set(SIMPL_WRAP_PYTHON "OFF")
set(SIMPL_USE_QtWebEngine "OFF")
set(SIMPLView_USE_STYLESHEETEDITOR "OFF")
set(ITKImageProcessing_ENABLE_SCIFIO "OFF")
set(BUILD_DEVHELPER OFF)
set(BUILD_TEST_FILTERS OFF)
set(DREAM3D_LINUX_SYSTEM "${CTEST_SITE}")
set(DREAM3D_BUILD_DEVHELPER "OFF")
set(DREAM3D_BUILD_VERSION_FILE "OFF")
set(DREAM3D_BUILD_TEST_FILTERS "ON")
set(CMAKE_CXX_STANDARD 11 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING "" FORCE)
set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
if("${BUILD_TYPE}" STREQUAL "")
    set(BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()


set(DREAM3D_SDK_VERSION 1.2.0.0)

set(BUILD_SHARED_LIBS ON CACHE BOOL "")
set(DREAM3D_SDK_ROOT "/opt/DREAM3D_SDK")

#--------------------------------------------------------------------------------------------------
# Always write out the compile_commands.json file to help out things like QtCreator and VS Code
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#--------------------------------------------------------------------------------------------------
# Python Version 3.6.7
# Python is needed if you want to build the documentation using Mkdocs which will produce
# nice looking documentation. If there is no python environment found then DREAM3D will 
# use the 'discount' library to generate the help files.
# A suitable Python and mkdocs were found.
set(PYTHON_EXECUTABLE "/usr/bin/python3.6")
set(MKDOCS_EXECUTABLE "/usr/local/lib/python3.6/dist-packages/mkdocs")
set(SIMPL_USE_DISCOUNT OFF)
set(SIMPL_USE_MKDOCS ON)

#--------------------------------------------------------------------------------------------------
# pybind11
set(pybind11_DIR "${DREAM3D_SDK_ROOT}/pybind11-2.2-${BUILD_TYPE}/share/cmake/pybind11" CACHE PATH "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${pybind11_DIR})

#--------------------------------------------------------------------------------------------------
# Discount
set(discount_DIR "${DREAM3D_SDK_ROOT}/discount-2.2.3-${BUILD_TYPE}/lib/cmake/discount" CACHE PATH "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${discount_DIR})

#--------------------------------------------------------------------------------------------------
# DREAM3D Data Folder Location
set(DREAM3D_DATA_DIR "${DREAM3D_SDK_ROOT}/DREAM3D_Data" CACHE PATH "")

#--------------------------------------------------------------------------------------------------
# Eigen Library Location
set(EIGEN_INSTALL "${DREAM3D_SDK_ROOT}/Eigen-3.3.5" CACHE PATH "")

#--------------------------------------------------------------------------------------------------
# HDF5 Library Location
set(HDF5_INSTALL "${DREAM3D_SDK_ROOT}/hdf5-1.10.3-${BUILD_TYPE}" CACHE PATH "")
set(HDF5_DIR "${DREAM3D_SDK_ROOT}/hdf5-1.10.3-${BUILD_TYPE}/share/cmake/hdf5" CACHE PATH "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${HDF5_DIR})

#--------------------------------------------------------------------------------------------------
# Intel Threading Building Blocks Library
set(SIMPL_USE_MULTITHREADED_ALGOS ON CACHE BOOL "")
set(TBB_INSTALL_DIR "${DREAM3D_SDK_ROOT}/tbb2018_20180618oss/tbb2018_20180618oss" CACHE PATH "")
set(TBB_DIR "${DREAM3D_SDK_ROOT}/tbb2018_20180618oss/tbb2018_20180618oss/cmake" CACHE PATH "")
set(TBB_ARCH_TYPE "intel64" CACHE STRING "")

#--------------------------------------------------------------------------------------------------
# haru
set(libharu_DIR "${DREAM3D_SDK_ROOT}/haru-2.0.0-${BUILD_TYPE}/cmake/libharu" CACHE PATH "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${libharu_DIR})

#--------------------------------------------------------------------------------------------------
# ITK Library Location
set(ITK_DIR "${DREAM3D_SDK_ROOT}/ITK-4.13.1-${BUILD_TYPE}" CACHE PATH "")
set(DREAM3D_USE_ITK "ON")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ITK_DIR})

#--------------------------------------------------------------------------------------------------
# Qt5 5.11.1 Library
set(Qt5_DIR "/opt/qt511/lib/cmake/Qt5" CACHE PATH "")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${Qt5_DIR})

#--------------------------------------------------------------------------------------------------
# Qwt 6.1.3 Library
set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/qwt-6.1.3-5.11.1" CACHE PATH "")


# Helper macro to write the initial cache.
macro(write_cache)
  set(cache_build_type "")
  set(cache_make_program "")
  set(cache_build_type CMAKE_BUILD_TYPE:STRING=${CTEST_CONFIGURATION_TYPE})
  if(CMAKE_MAKE_PROGRAM)
    set(cache_make_program CMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM})
  endif()
  file(WRITE ${CTEST_BINARY_DIRECTORY}/CMakeCache.txt "
      ${cache_build_type}
      ${cache_make_program}
      BOOST_ROOT:PATH=${BOOST_ROOT}
      BUILD_SHARED_LIBS:BOOL=ON
      BUILDNAME:STRING=${CTEST_BUILD_NAME}
      CMAKE_CXX_STANDARD_REQUIRED:BOOL=ON
      CMAKE_CXX_STANDARD:STRING=11
      CTEST_USE_LAUNCHERS:BOOL=${CTEST_USE_LAUNCHERS}
      DART_TESTING_TIMEOUT:STRING=${CTEST_TEST_TIMEOUT}
      DART_TESTING_TIMEOUT:STRING=60000
      discount_DIR=${discount_DIR}
      DREAM3D_BUILD_TEST_FILTERS:BOOL=${DREAM3D_BUILD_TEST_FILTERS}
      DREAM3D_BUILD_VERSION_FILE:BOOL=${DREAM3D_BUILD_VERSION_FILE}
      DREAM3D_DATA_DIR:PATH=${DREAM3D_DATA_DIR}
      DREAM3D_DATA_ROOT:PATH=${DREAM3D_DATA_DIR}
      DREAM3D_EXTRA_PLUGINS:STRING=${DREAM3D_EXTRA_PLUGINS}
      DREAM3D_LINUX_SYSTEM:STRING=${DREAM3D_LINUX_SYSTEM}
      EIGEN_INSTALL:PATH=${EIGEN_INSTALL}
      HDF5_DIR:PATH=${HDF5_INSTALL}/share/cmake
      HDF5_INSTALL:PATH=${HDF5_INSTALL}
      ITK_DIR:PATH=${ITK_DIR}
      libharu_DIR=${libharu_DIR}
      pybind11_DIR=${pybind11_DIR}
      QT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
      Qt5_DIR:PATH=${Qt5_DIR}
      QWT_INSTALL:PATH=${QWT_INSTALL}
      SIMPL_USE_MULTITHREADED_ALGOS:BOOL=${SIMPL_USE_MULTITHREADED_ALGOS}
      SIMPL_USE_QtWebEngine:BOOL=${SIMPL_USE_QtWebEngine}
      SIMPL_WRAP_PYTHON=${SIMPL_WRAP_PYTHON}
      SIMPLView_BUILD_CLI_TOOLS:BOOL=ON
      SIMPLView_BUILD_DevHelper:BOOL=${BUILD_DEVHELPER}
      SIMPLView_USE_STYLESHEETEDITOR:BOOL=${SIMPLView_USE_STYLESHEETEDITOR}
      SITE:STRING=${CTEST_SITE}
      TBB_ARCH_PLATFORM:STRING=${TBB_ARCH_PLATFORM}
      TBB_DIR:PATH=${TBB_DIR}
      ")
endmacro(write_cache)

if(NOT EXISTS "${CTEST_BINARY_DIRECTORY}/CMakeCache.txt")
  set(dashboard_fresh 1)
  write_cache()
endif()


ctest_start("Continuous")
ctest_configure(OPTIONS "${cfg_options}")
ctest_build()
ctest_test(RETURN_VALUE rv)
ctest_submit()

if(NOT rv EQUAL 0)
  message(FATAL_ERROR "Test failures occurred.")
endif()
