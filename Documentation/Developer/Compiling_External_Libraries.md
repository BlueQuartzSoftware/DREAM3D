Compiling DREAM3D {#compilingexternallibraries}
========

# Preliminaries #

A Compiler suite for your Operating System (Visual Studio, Xcode, GCC). The following table lists the compilers that DREAM3D is commonly compiled against.

| Compiler Version | Notes |  
|  ------	| ------	|  
| Visual Studio 2013 Pro & Express SP1 | 64 Bit builds |  
| Xcode 5.1.1 (OS X Version 10.8 & 10.9) | 64 bit Intel Only |  
| GCC 4.7.x (Ubuntu 12.04) | 64 Bit Intel Only |  

## Note for MinGW/MSys/Cygwin ##

Due to incompatibilities of HDF5 and these environments we can not support compiling DREAM3D using these environments. Visual Studio Community 2013 is Free for anyone to use and has no restrictions on the software produced and its distribution.




# External Dependencies (Recommended Order of Installation) #

|  Package   | Minimum Version | Download Location |  
|  ------	| ------	| ------	|  
| Git | 1.97.x | [http://www.git-scm.com](http://www.git-scm.com) |  
| CMake | 3.1.3 | [http://www.cmake.org/cmake/resources/software.html](http://www.cmake.org/cmake/resources/software.html) |
| Doxygen | 1.8.9 | [http://www.stack.nl/~dimitri/doxygen/download.html](http://www.stack.nl/~dimitri/doxygen/download.html) |  
| HDF5 | 1.8.14 | [http://www.hdfgroup.org/HDF5/release/obtain5.html](http://www.hdfgroup.org/HDF5/release/obtain5.html) |  
| Boost | 1.57.0 | [http://www.boost.org](http://www.boost.org) |  
| Eigen | 3.2.4 | [Eigen Home Page](http://eigen.tuxfamily.org/index.php?title=Main_Page) |  
| Intel Threading Building Blocks | tbb42_20141204oss | [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download) |  
| Qt 5.4.x | 5.4.1 (Qt 4.x will NOT work) |  The developer should obtain a precompiled package from [http://www.qt.io](http://www.qt.io) and install that version rather than trying to compile Qt itself. |
| Qwt | 6.1.2 | Obtain from source forge |  


# DREAM3D SDK Availability #

The DREAM3D developers keep a downloadable _DREAM3D\_SDK_ for Visual Studio 2013 that contains 64 bit builds of **ALL** the dependent libraries (Except Qt 5 and ITK), CMake and Doxygen as a single installer. Please send an email to _dream3d@bluequartz.net_ to request this installer.


# Setting Up the Environment #

The developer is urged to keep two distinct folder hierarchies on their computer. The first is a folder that contains all the source codes to all the various libraries and DREAM3D itself, lets call this **Workspace**. The second is a folder where the developer will _INSTALL_ all the libraries that they will compile, let's call this the **DREAM3D_SDK**.  The general flow is to download the source into the **Workspace** directory, configure it, build it and then install the libraries into the **DREAM3D_SDK** folder. The single exception to this is Qt on Windows. For this very specific case build Qt in its final _installation_ location because once Qt is built on Windows platforms it **CAN NOT** be moved to another location. Let's look at an example on Windows:

Our **Workspace** is location at 

	C:\Users\[user_id]\Workspace
	
Our **DREAM3D\_SDK** is located at 

	C:\Developer\DREAM3D_SDK

With this environment setup proceed to download and install Git, CMake and Doxygen into the **DREAM3D\_SDK** directory. Then proceed to download, build and install the rest of the dependent libraries.

# Notes for Building Libraries #

# Building HDF5 #

+ _BUILD_SHARED_LIBS_ = ON
+ _HDF5_ENABLE_DEPRECATED_SYMBOLS_ = ON
+ _HDF5_USE_16_API_DEFAULT_ = ON
+ Export the "HDF5_INSTALL" environment variable to make configuring DREAM3D with CMake easier.

## OS X Special Notes ##

With the introduction of proper CMakeConfig support into HDF5 1.8.14 there now exists the need to build and install Debug and Release libraries into separate installation locations. The **recommended** way to now build HDF5 on OS X is to add a "-Debug" or "-Release" to the CMAKE_INSTALL_PREFIX for each build (depending on the CMAKE_BUILD_TYPE variable).

+ CMAKE_INSTALL_PREFIX=/opt/DREAM3D_SDK/hdf5-1.8.14-Debug  (For Debug build)
+ CMAKE_INSTALL_PREFIX=/opt/DREAM3D_SDK/hdf5-1.8.14-Release  (For Release Build)

**This setup becomes critically important when the "ImageProcessing" plugin or ITK in general is used in DREAM3D. This ensures that a consistent HDF5 library is linked into the plugin and DREAM3D.**

# Building Boost #

In order to build boost 1.57.0 one needs to use the "b2" command.

	Download the boost-1.57.0.zip file for windows.
	Decompress and open up a Visual Studio x64 command prompt.
	CD into the boost-1.57.0 directory.
	./bootstrap
	C:\Users\mjackson\Workspace\boost_1_57_0>.\b2 --build-dir=Build toolset=msvc  --prefix=C:/DREAM3D_SDK/boost-1.57.0 variant=debug,release link=static threading=multi address-model=64 install
	
That should build and install the complete boost library set with Debug and Release libraries with static linking.



# Building Eigen #

+ The project is configured with CMake and there is actually no compiling to do for Eigen. Just configure and then execute the "install" target for your IDE or makefiles.
+ Export the "EIGEN_INSTALL" environment variable.

## Windows with Visual Studio and Ninja ##

 If you use Ninja to configure and build Eigen there are a few steps. this is what worked for me.

+ C:\Users\mjackson\Workspace\eigen-3.2.4\zRel>cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/DREAM3D_SDK/Eigen-3.2.4 -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF ../
+ Then create an empty file called "DartConfiguration.tcl" in the build directory.
+ Run the CMake command again
+ ninja install

Repeat the process for a Debug build.


# Threading Building Blocks #

+ Download TBB from [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download)
+ Install into the **_DREAM3D\_SDK** location.
+ There is a script in the DREAM3D/Support/Scripts to build TBB on OS X due to rpath issues and the prebuilt binaries for OS X.


# Qt 5.4.1 #
Download Qt from [http://www.qt.io](http://www.qt.io). One can usually download and install the "Qt SDK" which will have everything you need. 

**Windows Visual Studio Users** The Qt-Project provides pre-built binaries for Qt for Visual Studio (VS) BUT they are ONLY 64 bit and they are ONLY for Visual Studio 2013 and ONLY for a specific version of Visual Studio 2013. If your version of Visual Studio does NOT match theirs, down to the exact security patches and service packs you **will** get errors due to mismatched "manifests", C/C++ runtime DLL issues and a few other things. 

## Building Qt 5.4.x on Windows ##

Don't. Download the prebuilt binaries from qt.io

# Building Qwt #

Build Qwt 6.1.2 which is the first version to be compatible with Qt 5.4.1

+ Download version 6.1.2 for Qt 5.4 compatibility
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

	QWT_INSTALL_PREFIX    = /Users/Shared/Toolkits/qwt-$$QWT_VERSION

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


