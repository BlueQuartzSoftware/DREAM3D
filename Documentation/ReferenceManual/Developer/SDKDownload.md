Download the DREAM.3D SDK {#sdk_download}
========


DREAM.3D depends on several 3rd party libraries to be installed and ready to link against. This section will detail the needed libraries and utilities and where to obtain them or how to compile them.

# External Dependencies (Recommended Order of Installation) #

|  Package   | Minimum Version | Download Location |
|  ------	| ------	| ------	|
| Git | 2.8.x | [http://www.git-scm.com](http://www.git-scm.com) |
| CMake | 3.5.1 | [http://www.cmake.org/cmake/resources/software.html](http://www.cmake.org/cmake/resources/software.html) |
| Doxygen | 1.8.11 | [http://www.stack.nl/~dimitri/doxygen/download.html](http://www.stack.nl/~dimitri/doxygen/download.html) |
| HDF5 | 1.8.16 | [http://www.hdfgroup.org/HDF5/release/obtain5.html](http://www.hdfgroup.org/HDF5/release/obtain5.html) |
| Boost | 1.60.0 | [http://www.boost.org](http://www.boost.org) |
| Eigen | 3.2.8 | [Eigen Home Page](http://eigen.tuxfamily.org/index.php?title=Main_Page) |
| Intel Threading Building Blocks | tbb44_20151115oss | [http://threadingbuildingblocks.org/download](http://threadingbuildingblocks.org/download) |
| Qt 5.6.0 | 5.6.0 (Qt 4.x will NOT work) |  The developer should obtain a precompiled package from [http://www.qt.io](http://www.qt.io) and install that version rather than trying to compile Qt itself. Visual Studio uses should get Qt 5.6.0 at the minimum.  |
| Qwt | 6.1.2 | Obtain from [https://sourceforge.net/projects/qwt/files](https://sourceforge.net/projects/qwt/files) |
| ITK | 4.9.1 | [ITK.org](http://www.itk.org) 4.9.1 is the earliest version compatible with HDF5 1.8.16 |



## Windows with Visual Studio 2013 Pro/Community (CE) ##

The DREAM.3D developers keep a downloadable _DREAM3D\_SDK_ for **Visual Studio 2013** that contains 64 bit builds of **ALL** the dependent libraries and applications except for Qt 5 and ITK. The _DREAM3D\_SDK_ can be downloaded at [http://dream3d.bluequartz.net/binaries/SDK/](http://dream3d.bluequartz.net/binaries/SDK/). As of July 2016 there is no SDK for Visual Studio 2015. The developer is responsible for compiling all 3rd party libraries for Visual Studio 2015.

**The developer is strongly urged to utilize the available SDK as everything is included except a git install**. Unless the developer has a special need to compile everything, it is best to use the precompiled SDK.


## OS X ##

For OS X developers a shell script has been developed that will download the source binaries, unpack them and then compile the libraries. The shell script can be run through the "Terminal.app" application located at **/Applications/Utilities/Terminal.app**

	[user] $ DREAM3D/Support/Scripts/OSX_Build_Scripts/Build_SDK.sh

The shell script will ask for the user's login password in order to install the DREAM3D

## Linux ##



