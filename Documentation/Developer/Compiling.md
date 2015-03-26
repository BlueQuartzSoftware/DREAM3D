Compiling DREAM3D {#compilingdream3d}
========

## Preliminaries ##

A Compiler suite for your Operating System (Visual Studio, Xcode, GCC). The following table lists the compilers that DREAM3D is commonly compiled against.

| Compiler Version | Notes |  
|  ------	| ------	|  
| Visual Studio 2008 Standard SP1 | 32 and 64 Bit builds |  
| Visual Studio 2010 Pro SP1 | 32 and 64 Bit builds |  
| Visual Studio 2012 Pro & Express SP1 | 32 and 64 Bit builds |  
| Visual Studio 2013 Pro & Express SP1 | 64 Bit builds |  
| Xcode 3.2.6 (OS X Version 10.6.8) | 64 bit Intel Only |  
| Xcode 4.6.2 (OS X Version 10.7 & 10.8) | 64 bit Intel Only |  
| Xcode 5.1.1 (OS X Version 10.8 & 10.9) | 64 bit Intel Only |  
| GCC 4.2.2 (CentOS 6.4) | 64 Bit Intel Only |  
| GCC 4.7.x (Ubuntu 12.04) | 64 Bit Intel Only |  

### Note for MinGW/MSys/Cygwin ###

Due to incompatibilities of HDF5 and these environments we can not support compiling DREAM3D using these environments. Visual Studio Express is Free for anyone to use and has no restrictions on the software produced and its distribution.




## External Dependencies (Recommended Order of Installation) ##

|  Package   | Minimum Version | Download Location |  
|  ------	| ------	| ------	|  
| Git | 1.97.x | [http://www.git-scm.com](http://www.git-scm.com) |  
| CMake | 3.1.0 | [http://www.cmake.org/cmake/resources/software.html](http://www.cmake.org/cmake/resources/software.html) |
| Doxygen | 1.8.7 | [http://www.stack.nl/~dimitri/doxygen/download.html](http://www.stack.nl/~dimitri/doxygen/download.html) |  
| HDF5 | 1.8.14 | [http://www.hdfgroup.org/HDF5/release/obtain5.html](http://www.hdfgroup.org/HDF5/release/obtain5.html) |  
| Boost | 1.57.0 | [http://www.boost.org](http://www.boost.org) |  
| Eigen | 3.2.4 | [Eigen Home Page](http://eigen.tuxfamily.org/index.php?title=Main_Page) |  
| Intel Threading Building Blocks | tbb42_20141204oss | [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download) |  
| Qt | 5.4.0 (Qt 4.x will NOT work) | [http://www.qt-project.org](http://www.qt-project.org) |  
| Qwt | 6.1.2 | Obtain from source forge |  


## DREAM3D SDK Availability ##

The DREAM3D developers keep a downloadable _DREAM3D\_SDK_ for Visual Studio 2013 that contains 64 bit builds of **ALL** the dependent libraries, CMake and Doxygen as a single installer. Please send an email to _dream3d@bluequartz.net_ to request this installer.


## Setting Up the Environment ##

The developer is urged to keep two distinct folder hierarchies on their computer. The first is a folder that contains all the source codes to all the various libraries and DREAM3D itself, lets call this **Workspace**. The second is a folder where the developer will _INSTALL_ all the libraries that they will compile, let's call this the **DREAM3D_SDK**.  The general flow is to download the source into the **Workspace** directory, configure it, build it and then install the libraries into the **DREAM3D_SDK** folder. The single exception to this is Qt on Windows. For this very specific case build Qt in its final _installation_ location because once Qt is built on Windows platforms it **CAN NOT** be moved to another location. Let's look at an example on Windows:

Our **Workspace** is location at 

	C:\Users\[user_id]\Workspace
	
Our **DREAM3D\_SDK** is located at 

	C:\Developer\DREAM3D_SDK

With this environment setup proceed to download and install Git, CMake and Doxygen into the **DREAM3D\_SDK** directory. Then proceed to download, build and install the rest of the dependent libraries.

## Notes for Building Libraries ##

### Building HDF5 ###

+ _BUILD_SHARED_LIBS_ = ON
+ _HDF5_ENABLE_DEPRECATED_SYMBOLS_ = ON
+ _HDF5_USE_16_API_DEFAULT_ = ON
+ Export the "HDF5_INSTALL" environment variable to make configuring DREAM3D with CMake easier.

### Building Boost ###

In order to build boost 1.57.0 one needs to use the "b2" command.

	Download the boost-1.57.0.zip file for windows.
	Decompress and open up a Visual Studio x64 command prompt.
	CD into the boost-1.57.0 directory.
	./bootstrap
	C:\Users\mjackson\Workspace\boost_1_57_0>.\b2 --build-dir=Build toolset=msvc  --prefix=C:/DREAM3D_SDK/boost-1.57.0 variant=debug,release link=static threading=multi address-model=64 install
	
That should build and install the complete boost library set with Debug and Release libraries with static linking.



### Building Eigen ###

+ The project is configured with CMake and there is actually no compiling to do for Eigen. Just configure and then execute the "install" target for your IDE or makefiles.
+ Export the "EIGEN_INSTALL" environment variable.

#### Windows with Visual Studio and Ninja ####

 If you use Ninja to configure and build Eigen there are a few steps. this is what worked for me.

+ C:\Users\mjackson\Workspace\eigen-3.2.4\zRel>cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/DREAM3D_SDK/Eigen-3.2.4 -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF ../
+ Then create an empty file called "DartConfiguration.tcl" in the build directory.
+ Run the CMake command again
+ ninja install

Repeat the process for a Debug build.


### Threading Building Blocks ###

+ Download TBB from [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download)
+ Install into the **_DREAM3D\_SDK** location.
+ There is a script in the DREAM3D/Support/Scripts to build TBB on OS X due to rpath issues and the prebuilt binaries for OS X.


### Qt ###
Download Qt from [http://www.qt.io](http://www.qt.io). One can usually download and install the "Qt SDK" which will have everything you need. 

**Windows Visual Studio Users** The Qt-Project provides pre-built binaries for Qt for Visual Studio (VS) BUT they are ONLY 64 bit and they are ONLY for Visual Studio 2013 and ONLY for a specific version of Visual Studio 2013. If your version of Visual Studio does NOT match theirs, down to the exact security patches and service packs you **will** get errors due to mismatched "manifests", C/C++ runtime DLL issues and a few other things. 

### Building Qt on Windows ###

Don't. Download the prebuilt binaries from qt.io

### Building Qwt ###

Build Qwt 6.1.2 which is the first version to be compatible with Qt 5.4.0

## Building DREAM3D ##

After you have acquired the source code for DREAM3D you can use CMake to configure it for your development environment and then build it. If you have used the DREAM3D SDK Install a file will have been installed into the top level of the DREAM3D_SDK folder that has all the necessary paths for CMake to use. This file may look something like the folllowing:

	set(BUILD_SHARED_LIBS ON CACHE BOOL "")
	set(DREAM3D_SDK_ROOT "/Users/Shared/Toolkits")
	set(DREAM3D_DATA_DIR ${DREAM3D_SDK_ROOT}/DREAM3D_Data CACHE PATH "")
	set(QT_QMAKE_EXECUTABLE "${DREAM3D_SDK_ROOT}/Qt5.4.0/bin/qmake" CACHE FILEPATH "")
	set(EIGEN_INSTALL "${DREAM3D_SDK_ROOT}/Eigen-3.2.4" CACHE PATH "")
	set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/Qwt-6.1.2" CACHE PATH "")
	set(HDF5_INSTALL "${DREAM3D_SDK_ROOT}/hdf5-1.8.14" CACHE PATH "")
	set(BOOST_ROOT "${DREAM3D_SDK_ROOT}/boost-1.57.0" CACHE PATH "")
	set(TBB_INSTALL_DIR  "${DREAM3D_SDK_ROOT}/tbb42_20141204oss" CACHE PATH "")
	set(DREAM3D_USE_MULTITHREADED_ALGOS ON CACHE BOOL "")
	set(TBB_ARCH_TYPE "intel64" CACHE STRING "")
	set(ITK_DIR "${DREAM3D_SDK_ROOT}/ITKBuild" CACHE PATH "")

**Note: The paths may be different on your system**

When running CMake (either from the command line or from teh GUI) be sure to set the CMake variable "DREAM3D_SDK="/Users/Shared/Toolkits" (Or the correct path for your system). This should be all that is needed for a successful cmake configure run.

#### CMake configuration from the command line ####
	cmake -DDREAM3D_SDK=/Users/Shared/Toolkits -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON "-DDREAM3D_EXTRA_PLUGINS=ImageProcessing" ../


Once you have configured DREAM3D for building using CMake then simply open the generated solution file (DREAM3DProj.sln), Xcode project (DREAM3DProj.xcodeproject) or use _make_ to compile DREAM3D.

## Building a Release of DREAM3D

 These instructions are for those wanting to create an official distribution of DREAM3D for the main web site. Here is the list of things to do in the order they need to be performed:

- Open the Top level CMakeLists.txt file and adjust the _CMP_VERSION_MAJOR_ to the desired version for this release. Refer to the documentation for determining the version number and how/what to set it to.
- Save the CMakeLists.txt file
- Perform one last "git commit ...." to commit this version number.
- Run CMake to generate new sources with the latest version number.
- Compile DREAM3D one last time.
- Create the downloadable package by either running the **PACKAGE** project in Visual Studio or running _make package_ on Makefile based systems.
- Upload the binaries to the website
- Update the main web page with the new version and download links.
- Update the rss feed file with the new version and download links.



