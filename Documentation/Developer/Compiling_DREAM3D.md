Compiling DREAM3D {#compilingdream3d}
========

# Preliminaries #

A Compiler suite for your Operating System (Visual Studio, Xcode, GCC). The following table lists the compilers that DREAM3D is commonly compiled against.

| Compiler Version | Notes |  
|  ------	| ------	|  
| Visual Studio 2013 Pro & Express SP1 | 64 Bit builds |  
| Xcode 5.1.1 (OS X Version 10.8 & 10.9) | 64 bit Intel Only |  
| GCC 4.7.x (Ubuntu 12.04) | 64 Bit Intel Only |  

## Note for MinGW/MSys/Cygwin ##

Due to incompatibilities of HDF5 and these environments we can not support compiling DREAM3D using these environments. Visual Studio Express is Free for anyone to use and has no restrictions on the software produced and its distribution.


# Building DREAM3D #

## Getting the Source Code ##

Use Git to clone the repository located [http://github.com/dream3d/dream3d](http://github.com/dream3d/dream3d).

## Getting Unit Test Data Files ##

Use Git to clone the repository located [http://github.com/dream3d/DREAM3D_Data](http://github.com/dream3d/DREAM3D_Data) into a location on your hard drive. **Be sure to adjust the DREAM3D_DATA_DIR variable to the appropriate location**. See next section.

## Setting up CMake for Configuration ##

Once the source code is cloned onto your local hard drive and all the external library dependencies are all either installed or compiled the developer needs to create a **DREAM3D_SDK.cmake** text file located in a separate folder. The contents of this file describe where to find all the dependent libraries. For an OS X build the following is a typical file:

	#--------------------------------------------------------------------------------------------------
	# DREAM3D_SDK.cmake Contents for OS X
	set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
	if("${BUILD_TYPE}" STREQUAL "")
	    set(BUILD_TYPE "Release" CACHE STRING "" FORCE)
	endif()
	message(STATUS "The Current Build type being used is ${BUILD_TYPE}")

	set(BUILD_SHARED_LIBS ON CACHE BOOL "")
	set(DREAM3D_SDK_ROOT "/Users/Shared/Toolkits")
	set(DREAM3D_DATA_DIR ${DREAM3D_SDK_ROOT}/DREAM3D_Data CACHE PATH "")
	set(Qt5_DIR "${DREAM3D_SDK_ROOT}/Qt5.4.1/5.4/clang_64/lib/cmake/Qt5" CACHE PATH "")
	set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/qwt-6.1.2" CACHE PATH "")
	set(EIGEN_INSTALL "${DREAM3D_SDK_ROOT}/Eigen-3.2.2" CACHE PATH "")
	set(BOOST_ROOT "${DREAM3D_SDK_ROOT}/boost-1.57.0" CACHE PATH "")
	set(TBB_INSTALL_DIR  "${DREAM3D_SDK_ROOT}/tbb42_20140601oss" CACHE PATH "")
	set(DREAM3D_USE_MULTITHREADED_ALGOS ON CACHE BOOL "")
	set(TBB_ARCH_TYPE "intel64" CACHE STRING "")
	set(ITK_DIR "${DREAM3D_SDK_ROOT}/ITK-4.7.1-${BUILD_TYPE}")
	set(HDF5_INSTALL "${DREAM3D_SDK_ROOT}/hdf5-1.8.14-${BUILD_TYPE}")

	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ITK_DIR} ${Qt5_DIR} ${HDF5_INSTALL})
	set(DREAM3D_EXTRA_PLUGINS "ImageProcessing")
	#--------------------------------------------------------------------------------------------------

	#--------------------------------------------------------------------------------------------------
	# DREAM3D_SDK.cmake contents for Windows
	set(BUILD_SHARED_LIBS ON CACHE BOOL "")
	set(DREAM3D_SDK_ROOT "C:/DREAM3D_SDK")
	set(DREAM3D_DATA_DIR ${DREAM3D_SDK_ROOT}/DREAM3D_Data CACHE PATH "")
	set(Qt5_DIR ${DREAM3D_SDK_ROOT}/Qt5.4.1/5.4/msvc2013_64_opengl/lib/cmake/Qt5 CACHE PATH "")
	set(EIGEN_INSTALL "${DREAM3D_SDK_ROOT}/Eigen-3.2.4" CACHE PATH "")
	set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/Qwt-6.1.2" CACHE PATH "")
	set(DOXYGEN_EXECUTABLE "${DREAM3D_SDK_ROOT}/doxygen-1.8.9.1.windows.x64.bin/doxygen.exe" CACHE FILEPATH "")
	set(HDF5_INSTALL "${DREAM3D_SDK_ROOT}/hdf5-1.8.14" CACHE PATH "")
	set(BOOST_ROOT "${DREAM3D_SDK_ROOT}/boost-1.57.0" CACHE PATH "")
	set(TBB_INSTALL_DIR  "${DREAM3D_SDK_ROOT}/tbb43_20141204oss" CACHE PATH "")
	set(DREAM3D_USE_MULTITHREADED_ALGOS ON CACHE BOOL "")
	set(TBB_ARCH_TYPE "intel64" CACHE STRING "")

	set(ITK_DIR "${DREAM3D_SDK_ROOT}/ITK-4.7.1")
	set(DREAM3D_EXTRA_PLUGINS "ImageProcessing" CACHE STRING "")
	# Append to CMake's search PATH
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${Qt5_DIR} ${ITK_DIR} ${OpenBLAS_DIR} ${HDF5_INSTALL}/cmake/hdf5)
	#--------------------------------------------------------------------------------------------------


Note that if you are on Linux or use something like MacPorts to install all the dependencies then you can substitute the appropriate paths instead of reusing the DREAM3D_SDK_ROOT variable. On linux, for example, if installing a dependency from a package-manager like apt-get one would set most of the installation locations to "/usr/". 

**Note: The paths may be different on your system**

When running CMake (either from the command line or from the GUI) be sure to set the CMake variable "DREAM3D_SDK=/Users/Shared/Toolkits" (Or the correct path for your system). This should be all that is needed for a successful cmake configuration run.

## CMake configuration from the command line ##

	cmake -DDREAM3D_SDK=/Users/Shared/Toolkits -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON "-DDREAM3D_EXTRA_PLUGINS=ImageProcessing" ../


Once you have configured DREAM3D for building using CMake then simply open the generated solution file (DREAM3DProj.sln), Xcode project (DREAM3DProj.xcodeproject) or use _make_ or _ninja_ from a terminal to compile DREAM3D.

