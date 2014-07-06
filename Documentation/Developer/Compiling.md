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
| Git | 1.7.x | [http://www.git-scm.com](http://www.git-scm.com) |  
| CMake | 3.0.0 | [http://www.cmake.org/cmake/resources/software.html](http://www.cmake.org/cmake/resources/software.html) |
| Doxygen | 1.8.7 | [http://www.stack.nl/~dimitri/doxygen/download.html](http://www.stack.nl/~dimitri/doxygen/download.html) |  
| HDF5 | 1.8.13 | [http://www.hdfgroup.org/HDF5/release/obtain5.html](http://www.hdfgroup.org/HDF5/release/obtain5.html) |  
| Boost | 1.56.0 | [http://www.boost.org](http://www.boost.org) |  
| Eigen | 3.1.2 | [Eigen Home Page](http://eigen.tuxfamily.org/index.php?title=Main_Page) |  
| Intel Threading Building Blocks | tbb42_20140122oss | [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download) |  
| Qt | 4.8.6 (Qt 5.x will NOT work) | [http://www.qt-project.org](http://www.qt-project.org) |  
| Qwt | 5.2.2 | git clone git://scm.bluequartz.net/support-libraries/qwt.git qwt |  


## DREAM3D SDK Availability ##

The DREAM3D developers keep a downloadable _DREAM3D\_SDK_ for Visual Studio 2010, 2012 and 2013 that contains 64 bit builds of **ALL** the dependent libraries, CMake and Doxygen as a single installer. Please send an email to _dream3d@bluequartz.net_ to request this installer.


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
+ _HDF5_ENABLE_DEPRECATED_SYMBOLS_ = OFF
+ _HDF5_USE_16_API_DEFAULT_ = OFF
+ Export the "HDF5_INSTALL" environment variable to make configuring DREAM3D with CMake easier.

### Building Boost ###

+ DREAM3D only requires a few of Boost's headers for memory management. 
+ There is no need to build any of Boost's libraries if you do not have to.
+ Export the BOOST_ROOT environment variable

### Building Eigen ###

+ The project is configured with CMake and there is actually no compiling to do for Eigen. Just configure and then execute the "install" target for your IDE or makefiles.
+ Export the "EIGEN_INSTALL" environment variable

### Threading Building Blocks ###

+ Download TBB from [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download)
+ Install into the **_DREAM3D\_SDK** location.
+ There is a script in the DREAM3D/Support/Scripts to build TBB on OS X due to rpath issues and the prebuilt binaries for OS X.


### Qt ###
Download Qt from [http://www.qt-project.org](http://www.qt-project.org). One can usually download and install the "Qt SDK" which will have everything you need. 

**Windows Visual Studio Users** The Qt-Project provides pre-built binaries for Qt for Visual Studio (VS) BUT they are ONLY 32 bit and they are ONLY for Visual Studio 2008 and ONLY for a specific version of Visual Studio 2008. If your version of Visual Studio does NOT match theirs, down to the exact security patches and service packs you **will** get errors due to mismatched "manifests", C/C++ runtime DLL issues and a few other things. If you fall into the category of mismatching versions the easiest, though initially longest, solution is to build Qt from scratch. Or if you need 64 bit support.

### Building Qt on Windows ###
Download the source code from [http://www.qt-project.org](http://www.qt-project.org)  and decompress the archive into the final location where you want Qt. You **CAN NOT** move the Qt installation **AFTER** you finish building it so choose wisely. Once you have your location selected open up a "Visual Studio Command Prompt" for either a 32 bit or 64 bit build. Once the Command Prompt is opened, navigate to the source direction and issue the following command

    configure -confirm-license -opensource -debug-and-release -shared -fast -exceptions -stl -no-qt3support -nomake demos -nomake examples  -no-webkit -platform win32-msvc2010

and answer a few of the questions. That will run for a short while after which you can issue the command

    nmake

to build the rest of Qt. **Warning** Completely compiling Qt can take **8 HOURS** even on a very fast computer. Allow enough time to get this step completed. The configuration that we use above should take about 2-3 hours of compilation time on a multi-core/multi-cpu system.

### Building Qwt ###

Use Git to clone the Qwt source code repository:

	git clone git://scm.bluequartz.net/support-libraries/qwt.git qwt
    cd qwt

 And use CMake to configure Qwt for your development environment. After CMake has generated the project files, open the IDE or type "make install" on the command line to build and install Qwt

## Building DREAM3D ##

After you have acquired the source code for DREAM3D you can use CMake to configure it for your development environment and then build it. If you have used the DREAM3D SDK Install a file will have been installed into the top level of the DREAM3D_SDK folder that has all the necessary paths for CMake to use. This file may look something like the folllowing:

	set(BUILD_SHARED_LIBS ON CACHE BOOL "")
	set(DREAM3D_SDK_ROOT "/Users/Shared/Toolkits")
	set(DREAM3D_DATA_DIR ${DREAM3D_SDK_ROOT}/DREAM3D_Data CACHE PATH "")
	set(QT_QMAKE_EXECUTABLE "${DREAM3D_SDK_ROOT}/Qt-4.8.6/bin/qmake" CACHE FILEPATH "")
	set(EIGEN_INSTALL "${DREAM3D_SDK_ROOT}/Eigen-3.1.2" CACHE PATH "")
	set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/Qwt-5.2.2" CACHE PATH "")
	set(HDF5_INSTALL "${DREAM3D_SDK_ROOT}/hdf5-1.8.13" CACHE PATH "")
	set(BOOST_ROOT "${DREAM3D_SDK_ROOT}/boost-1.55.0" CACHE PATH "")
	set(TBB_INSTALL_DIR  "${DREAM3D_SDK_ROOT}/tbb42_20140601oss" CACHE PATH "")
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



