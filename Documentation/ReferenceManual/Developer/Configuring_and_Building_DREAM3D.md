Configuring and Building DREAM.3D {#configure_and_build_dream3d}
========
<a name="prerequisites">
## Prerequisites ##
</a>

These prerequisites will get your system set up with a DREAM.3D SDK and compiling DREAM.3D.

If you have already fulfilled all of these prerequisites, skip to the [Using DevHelper to Create a Filter](#creating_a_filter) section.

<a name="compiler_suite">
### Install a Compiler Suite ###
</a>

A compatible compiler needs to be installed on your system to be able to build DREAM.3D.

For more information, please visit [Installing a Compiler Suite](@ref compiler_suite).

<a name="downloading_dream3d">
### Download the Source Code for DREAM.3D ###
</a>

The source code for DREAM.3D must be downloaded using Git.

For more information, please visit [Downloading the Source Code for DREAM.3D](@ref source_download).

<a name="installing_sdk">
### Install the DREAM.3D SDK ###
</a>

Developers of DREAM.3D have several options to install a DREAM.3D SDK, including:
1. Using a script (Mac OS X and Linux only)
2. Downloading pre-made builds (Windows only)
3. Using the Superbuild build system.

For more information, please visit [Installing the DREAM.3D SDK](@ref sdk_install).

## Getting Unit Test Data Files ##

Use git to clone the [DREAM.3D data repository](http://github.com/dream3d/DREAM3D_Data). **Be sure to adjust the DREAM3D_DATA_DIR CMake variable to the appropriate location**. See the next section for more information.

## Setting Up DREAM.3D for CMake Configuration ##

Once the SDK is installed (Windows) or compiled (OS X, Linux), a file with the name DREAM3D_SDK.cmake can be found in the _DREAM3D\_SDK_ directory. This file defines all the installed locations for all of the dependent libraries.

Note that if you are on Linux or use an OS X package library (MacPorts, Homebrew, etc.) to install all the dependencies, you must substitute the appropriate paths instead of reusing the DREAM3D_SDK_ROOT variable. On Linux, for example, if installing a dependency from a package manager like apt-get, one would set most of the installation locations to "/usr/". 

**Note: The paths may be different on your system**

When running CMake (either from the command line or from the GUI) be sure to set the CMake variable "DREAM3D_SDK=/Users/Shared/DREAM3D_SDK" (Or the correct path for your system). This should be all that is needed for a successful CMake configuration run.

## CMake Configuration from the Command Line ##

	[user] $ cd DREAM3D
	[user] $ mkdir ../DREAM3D-Build && cd ../DREAM3D-Build
	[user] $ cmake -DDREAM3D_SDK=/Users/Shared/DREAM3D_SDK -DCMAKE_BUILD_TYPE=Debug ../DREAM3D

Once you have configured DREAM.3D using CMake, simply open the generated solution file (DREAM3DProj.sln), Xcode project (DREAM3DProj.xcodeproject) or use **make** from a terminal to compile DREAM.3D.

## CMake Configuration from the CMake GUI ##

![Starting CMake and selecting a Source and binary directory {OS X}](Images/CMakeConfigure-1.png)
	
![Setting the DREAM3D_SDK CMake variable before running configure for the first time.](Images/CMakeConfigure-2.png)

![Specifying the generator for an OS X Build.](Images/CMakeConfigure-3.png)

![Click configure and wait.](Images/CMakeConfigure-4.png)

You may need to click configure a few times until all red highlights are gone. Once all red highlighted rows are gone click the "Generate" button to generate your projects files.

## Note for MinGW/MSys/Cygwin ##

Due to incompatibilities of HDF5, compiling DREAM.3D using these environments is not supported. Visual Studio Community Edition 2013 is a freely available alternative with no restrictions on use.

---
**Previous Page**: [Installing the DREAM.3D SDK](@ref sdk_install).

**Next Page**: [Creating a Filter](@ref creating_a_filter).
