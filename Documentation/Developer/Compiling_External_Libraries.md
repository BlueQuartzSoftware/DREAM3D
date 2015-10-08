Compiling External Libraries {#compilingexternallibraries}
========

# Online SDK Installation Tutorial #

A tutorial is available for [Windows 7/8](http://dream3d.bluequartz.net/?p=954) and [OS X](http://dream3d.bluequartz.net/?p=930) that covers the installation of a DREAM3D_SDK using either prebuilt binaries (Windows) or a shell script (OS X). Please use those web sites. The instructions below are for special cases where using the precompiled binaries is not a viable option for the developers environment.

# Preliminaries #

A Compiler suite for your Operating System (Visual Studio, Xcode, GCC). The following table lists the compilers that DREAM3D is commonly compiled against.

| Compiler Version | Notes |  
|  ------	| ------	|  
| Visual Studio 2013 Pro, Express or Community SP4 | 64 Bit builds |  
| Xcode 5.1.1 (OS X Version 10.8 & 10.9) | 64 bit Intel Only |  
| GCC 4.7.x or Clang (Ubuntu 14.04) | 64 Bit Intel Only |  

## Note for MinGW/MSys/Cygwin ##

Due to incompatibilities of HDF5 and these environments we can not support compiling DREAM3D using these environments. Visual Studio Community 2013 is Free for anyone to use and has no restrictions on the software produced and its distribution.


# DREAM3D SDK Availability - PLEASE USE this option. #

The DREAM3D developers keep a downloadable _DREAM3D\_SDK_ for Visual Studio 2013 that contains 64 bit builds of **ALL** the dependent libraries and applications except for Qt 5 and ITK. The _DREAM3D\_SDK_ can be downloaded [here](http://dream3d.bluequartz.net/binaries/SDK/).

**The developer is strongly urged to utilize the available SDK as everything is included except a Git install**. Unless the developer has a special need to compile everything, it is best to use the precompiled SDK. 

# External Dependencies (Recommended Order of Installation) #

|  Package   | Minimum Version | Download Location |
|  ------	| ------	| ------	|
| Git | 1.97.x | [http://www.git-scm.com](http://www.git-scm.com) |
| CMake | 3.2.3 | [http://www.cmake.org/cmake/resources/software.html](http://www.cmake.org/cmake/resources/software.html) |
| Doxygen | 1.8.10 | [http://www.stack.nl/~dimitri/doxygen/download.html](http://www.stack.nl/~dimitri/doxygen/download.html) |
| HDF5 | 1.8.15 | [http://www.hdfgroup.org/HDF5/release/obtain5.html](http://www.hdfgroup.org/HDF5/release/obtain5.html) |
| Boost | 1.58.0 | [http://www.boost.org](http://www.boost.org) |
| Eigen | 3.2.4 | [Eigen Home Page](http://eigen.tuxfamily.org/index.php?title=Main_Page) |
| Intel Threading Building Blocks | tbb43_20150424oss | [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download) |
| Qt 5.4.x | 5.4.2 (Qt 4.x will NOT work) |  The developer should obtain a precompiled package from [http://www.qt.io](http://www.qt.io) and install that version rather than trying to compile Qt itself. Visual Studio uses should get Qt 5.4.2 at the minimum. Qt 5.4.0 is **known** to produce bad compiled code for 64 bit builds. |
| Qwt | 6.1.2 | Obtain from source forge. This is the first version to be compatible with Qt 5.4.x |


# Setting Up the Environment #

The developer is urged to keep two distinct folder hierarchies on their computer. The first is a folder that contains all the source codes to all the various libraries and DREAM3D itself, lets call this **Workspace**. The second is a folder where the developer will _INSTALL_ all the libraries that they will compile, let's call this the **DREAM3D_SDK**.  The general flow is to download the source into the **Workspace** directory, configure it, build it and then install the libraries into the **DREAM3D_SDK** folder. The single exception to this is Qt on Windows. For this very specific case build Qt in its final _installation_ location because once Qt is built on Windows platforms it **CAN NOT** be moved to another location. Let's look at an example on Windows:

Our **Workspace** is location at 

	C:\Users\[user_id]\Workspace
	
Our **DREAM3D\_SDK** is located at 

	C:\DREAM3D_SDK

With this environment setup proceed to download and install Git, CMake and Doxygen into the **DREAM3D\_SDK** directory. Then proceed to download, build and install the rest of the dependent libraries.

# CMake 3.2.3 #

[CMake Download Link](http://www.cmake.org/download/)

Download CMake for your system. Unpack and drop the folder into the **DREAM3D\_SDK** folder.

## Linux Notes ##

The default packages are usually not new enough from the package maintainers and you will need to download the prebuilt binaries for linux.


# DOxygen 1.8.10 #

[Doxygen Download Link](http://www.stack.nl/~dimitri/doxygen/download.html)

Download Doxygen for your system. Unpack and drop the folder into the **DREAM3D\_SDK** folder.

## OS X Notes ##
	
On OS X systems the Doxygen.app should be placed into /Applications instead of the **DREAM3D\_SDK** folder.

## Linux Notes ##

Use distributions package manager (apt-get on Ubuntu or yum on Red Hat systems) to install Oxygen.



# Building HDF5 #

[HDF5 Download link](http://www.hdfgroup.org/ftp/HDF5/current/src/hdf5-1.8.15-patch1.tar.gz)

Extract the archive to your **Workspace** location. Then using a terminal (for Linux & OS X) or CMake-GUI (for Windows) configure HDF5 to build using your system tools.

## Windows Notes ##

Use CMake to configure HDF5 for building with Visual Studio. The developer should select the _Visual Studio 12 2013 Win64_ generator type when configuring the build. This will build both Debug and Release from the same Visual Studio solution.

## OS X & Linux Notes ##

Use the following sequence of Terminal commands to configure and build both a Debug and Release version of HDF5 1.8.15


	[user] $ tar -xvzf hdf5-1.8.15-patch1.tar.gz
	[user] $ cd hdf5-1.8.15-patch1 && mkdir Debug && cd Debug
	[user] $ cmake -DCMAKE_BUILD_TYPE=Debug  -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/Users/Shared/DREAM3D_SDK/hdf5-1.8.15-Debug ../
	[user] $ make -j
	[user] $ make install
	[user] $ cd ../
	[user] $ mkdir Release && cd Release
	[user] $ cmake -DCMAKE_BUILD_TYPE=Release  -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/Users/Shared/DREAM3D_SDK/hdf5-1.8.15-Release ../
	[user] $ make -j
	[user] $ make install

With the introduction of proper CMakeConfig support into HDF5 1.8.14 there now exists the need to build and install Debug and Release libraries into separate installation locations. The **recommended** way to now build HDF5 on OS X is to add a "-Debug" or "-Release" to the CMAKE_INSTALL_PREFIX for each build (depending on the CMAKE_BUILD_TYPE variable).

**This setup becomes critically important when the "ImageProcessing" plugin or ITK in general is used in DREAM3D. This ensures that a consistent HDF5 library is linked into the plugin and DREAM3D.**

# Building Boost 1.58.0 #

[http://sourceforge.net/projects/boost/files/boost/1.58.0/ ]( http://sourceforge.net/projects/boost/files/boost/1.58.0/ )

Windows developers should select the *.zip file and OS X users should select the .tar.gz file.

## Windows Notes ##

	Download the boost-1.58.0.zip file for windows.
	Decompress and open up a Visual Studio x64 command prompt.
	CD into the boost-1.58.0 directory.
	./bootstrap
	C:\\Users\\mjackson\\Workspace\boost_1_57_0>.\b2 --build-dir=Build toolset=msvc  --prefix=C:/DREAM3D_SDK/boost-1.57.0 variant=debug,release link=static threading=multi address-model=64 install

## OS X Notes ##

	[user] $ tar -xvzf boost_1_58_0.tar.gz
	[user] $ cd boost_1_58_0
	[user] $ ./bootstrap.sh 
	[user] $ ./b2 --prefix=/Users/Shared/DREAM3D_SDK/boost-1.58.0 --build-dir=Build variant=release link=shared threading=multi install
	[user] $ 


## Linux Notes ##

The distribution of boost with most modern Linux distributions is usually good enough for our needs. Just use what ever version your distribution has.

# Building Eigen #

+ [http://eigen.tuxfamily.org/index.php?title=Main_Page](http://eigen.tuxfamily.org/index.php?title=Main_Page)
+ The project is configured with CMake and there is actually no compiling to do for Eigen. Just configure and then execute the "install" target for your IDE or makefiles.
+ Export the "EIGEN_INSTALL" environment variable.

## Notes ##

	[user] $ tar -xvzf eigen-eigen*.tar.gz
	[user] $ cd eigen-eigen-bdd17ee3b1b3
	[user] $ mkdir Build && cd Build
	[user] $ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/Users/Shared/DREAM3D_SDK/eigen-3.2.5 ../
	[user] $ make install

## Windows with Visual Studio and Ninja ##

 If you use Ninja to configure and build Eigen there are a few steps. this is what worked for me.

+ C:\\Users\\mjackson\\Workspace\\eigen-3.2.4\\zRel>cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/DREAM3D_SDK/Eigen-3.2.4 -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF ../
+ Then create an empty file called "DartConfiguration.tcl" in the build directory.
+ Run the CMake command again
+ ninja install

Repeat the process for a Debug build.


# Threading Building Blocks #

## Windows Notes ##
Download [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download) TBB precompiled binaries for Visual Studio. Extract the .zip file into the DREAM3D_SDK directory. You should end up with the path DREAM3D_SDK/tbb43_20150424oss/ if you did this correctly.

## OS X Notes ##

Within the DREAM3D/Support/Scripts/OSX_Build_Scripts directory is a shell script called "Build_TBB.sh". This script should be used to build TBB for OS X.
	
## Linux Notes ##

+ Download TBB from [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download)
+ Install into the **_DREAM3D\_SDK** location.


# Qt 5.4.x #
Download Qt from [http://www.qt.io](http://www.qt.io). One can usually download and install the "Qt SDK" which will have everything you need.

Install Qt into C:/DREAM3D_SDK/Qt5.4.1 (Windows) /Users/Shared/DREAM3D_SDK/Qt5.4.1 (OS X) or /opt/DREAM3D_SDK/Qt5.4.1 (Linux).

**Windows Visual Studio Users** The Qt-Project provides pre-built binaries for Qt for Visual Studio (VS) BUT they are ONLY 64 bit and they are ONLY for Visual Studio 2013 and ONLY for a specific version of Visual Studio 2013. If your version of Visual Studio does NOT match theirs, down to the exact security patches and service packs you **will** get errors due to mismatched "manifests", C/C++ runtime DLL issues and a few other things. 

# Building Qwt #

Build Qwt 6.1.2 which is the first version to be compatible with Qt 5.4.1

+ Download version [Qwt 6.1.2](http://sourceforge.net/projects/qwt/files/latest/download) for Qt 5.4 compatibility 
+ Modify the following files:

## Edit qwt-6.1.2/src/src.pro ##

This is only for OS X to build a proper framework.

replace the following line:

	CONFIG += lib_bundle

with this:

	CONFIG += lib_bundle
	QMAKE_LFLAGS_SONAME  *= -install_name $${QWT_INSTALL_PREFIX}/lib/qwt.framework/Versions/6/qwt

This will set correctly the full path to the framework that is built. If this is **NOT** performed then the final DREAM3D Packaging will not work correctly.

## Edit qwt-6.1.2/qwtconfig.pri ##

replace the following line:

	QWT_INSTALL_PREFIX    = /usr/local/qwt-$$QWT_VERSION

with this (OS X):

	QWT_INSTALL_PREFIX    = /Users/Shared/DREAM3D_SDK/qwt-$$QWT_VERSION

or this (Windows):

    QWT_INSTALL_PREFIX    = C:/DREAM3D_SDK/Qwt-$$QWT_VERSION

Adjust the path to place Qwt where all the other DREAM3D library dependencies are at.

## Building Qwt 6.1.2 ##

Use qmake from the Qt 5.4 distribution from the command line with the following commands executed from inside the qwt-6.1.2 directory

	qmake qwt.pro

and then for OS X:

	make
	make install

For Windows it is (From a Visual Studio X64 command prompt)

	nmake
	nmake install


# Create DREAM3D_SDK.cmake Configuration File #

The develop now needs to create a basic cmake file that describes the locations of all the various libraries that are needed by DREAM.3D that the developer either just installed or compiled. The contents of the file should be as follows with the appropriately adjusted paths for your operating system. Note the file can technically go any where you want. DREAM.3D developers typically keep the file in the DREAM3D_SDK folder.


	#--------------------------------------------------------------------------------------------------
	# These settings are specific to DREAM3D. DREAM3D needs these variables to
	# configure properly.
	set(CMAKE_CXX_FLAGS "-Wmost -Wno-four-char-constants -Wno-unknown-pragmas")
	set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
	if("${BUILD_TYPE}" STREQUAL "")
	    set(BUILD_TYPE "Release" CACHE STRING "" FORCE)
	endif()
	message(STATUS "The Current Build type being used is ${BUILD_TYPE}")
	set(BUILD_SHARED_LIBS ON CACHE BOOL "")
	set(DREAM3D_SDK_ROOT "/Users/Shared/DREAM3D_SDK")
	set(DREAM3D_DATA_DIR ${DREAM3D_SDK_ROOT}/DREAM3D_Data CACHE PATH "")
	set(Qt5_DIR "${DREAM3D_SDK_ROOT}/Qt5.4.1/5.4/clang_64/lib/cmake/Qt5" CACHE PATH "")
	set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/qwt-6.1.2" CACHE PATH "")
	#--------------------------------------------------------------------------------------------------
	# Eigen Library
	set(EIGEN_INSTALL "${DREAM3D_SDK_ROOT}/Eigen-3.2.2" CACHE PATH "")
	set(BOOST_ROOT "${DREAM3D_SDK_ROOT}/boost-1.58.0" CACHE PATH "")
	#--------------------------------------------------------------------------------------------------
	# Intel Threading Building Blocks Library
	set(SIMPL_USE_MULTITHREADED_ALGOS ON CACHE BOOL "")
	set(TBB_INSTALL_DIR  "${DREAM3D_SDK_ROOT}/tbb43_20150424oss" CACHE PATH "")
	set(TBB_ARCH_TYPE "intel64" CACHE STRING "")
	#--------------------------------------------------------------------------------------------------
	# HDF5 Library
	set(HDF5_INSTALL "${DREAM3D_SDK_ROOT}/hdf5-1.8.14-${BUILD_TYPE}")
	set(HDF5_DIR "${DREAM3D_SDK_ROOT}/hdf5-1.8.14-${BUILD_TYPE}/share/cmake/hdf5")
	#--------------------------------------------------------------------------------------------------
	# Update CMake Module Path with additional paths in order to better find the libraries.
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ITK_DIR} ${Qt5_DIR} ${HDF5_INSTALL})



At this point the developer should be ready to compile DREAM3D. Please see the next document for those instructions.
