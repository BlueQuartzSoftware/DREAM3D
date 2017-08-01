Configuring and Building DREAM.3D on Linux {#linux_configure_and_build_dream3d}
========

<a name="prerequisites">

## Prerequisites ##

</a>

These prerequisites need to be completed before configuring and building DREAM.3D.

If you have already fulfilled all of these prerequisites, skip to the [Setting Up DREAM.3D for CMake Configuration](#cmake_config) section.

<a name="compiler_suite">

### Install a Compiler Suite ###

</a>

A compatible compiler needs to be installed on your system to be able to build DREAM.3D.

For more information, please visit [Installing a Compiler Suite](http://dream3d.bluequartz.net/binaries/Help/DREAM3D/compiler_suite.html) (external link).

<a name="installing_sdk">

### Install the DREAM.3D SDK ###

</a>

Developers of DREAM.3D must use the Superbuild build system to install a DREAM.3D SDK.

For more information, please visit <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_Linux.md">Making an SDK</a>.

<a name="cmake_config">

## Setting Up DREAM.3D for CMake Configuration ##

</a>

Once the SDK is installed, a file with the name DREAM3D_SDK.cmake can be found in the DREAM3D_SDK directory at */opt/DREAM3D_SDK/DREAM3D_SDK.cmake*. This file defines all the installed locations for all of the dependent libraries.

**Note**: If you use a package library to install all the dependencies, you must substitute the appropriate paths instead of reusing the DREAM3D_SDK_ROOT variable.

## Checking Paths of Dependencies ##
Open the **DREAM3D_SDK.cmake** file in the **DREAM3D_SDK** folder.  Check that the path to every dependency is set correctly.  For example, the paths to Qt, HDF5, Boost, Qwt, and others may be written incorrectly and may need to be updated to whatever path each one is located at on the file system.

## Instructions ##
1. Open CMake and set the **Where is the source code** path to */home/[YOUR-HOME-FOLDER]/Workspace/DREAM3D*.
![](Images/OSX_source_code_path.png)

2. Set the **Where to build the binaries** path to */home/[YOUR-HOME-FOLDER]/Workspace/Builds/DREAM3D-Build/Debug*.
![](Images/OSX_build_binaries_debug.png)

3. We are going to create a CMake variable.  Press the **Add Entry** button.
![](Images/OSX_add_entry.png)

4. Set the **Name** to *DREAM3D_SDK*.  Set the **Type** to *PATH* and set the **Value** to */opt/DREAM3D_SDK*
![](Images/OSX_create_cmake_variable.png)

6. You should now have one variable, DREAM3D_SDK.
![](Images/OSX_cmake_before_config.png)

7. Press the **Configure** button in CMake.

8. Set the generator to be compatible with whatever method you'll be using to work with DREAM.3D.  For example:
    For command line use, set it to **Unix Makefiles** or, if installed, **Ninja**.  Any generator can be used for command line, but these are the recommended ones.
    
	Make sure **Use default native compilers** is checked.
![](Images/OSX_generator.png)

9. When the configuration is finished, make sure that **CMAKE_BUILD_TYPE** is set to *Debug*.

10. Press the **Configure** button in CMake again.  You may need to click **Configure** a few times until all the red highlights are gone.
![](Images/OSX_cmake_after_config.png)

11. Press the **Generate** button in CMake to generate the build files.

12. Repeat steps 1-10, except use path */home/[YOUR-HOME-FOLDER]/Workspace/Builds/DREAM3D-Build/Release* for step 2 and set **CMAKE_BUILD_TYPE** to *Release* in step 9.

### Using the Command Line (Advanced Users Only) ###
Navigate to the DREAM3D directory, make a build directory, and navigate into your newly created build directory.  Configure DREAM.3D.

	[user] $ cd DREAM3D
	[user] $ mkdir ../DREAM3D-Build && cd ../DREAM3D-Build
	[user] $ cmake -DDREAM3D_SDK=/opt/DREAM3D_SDK -DCMAKE_BUILD_TYPE=Debug ../DREAM3D

## Building DREAM.3D ##
Once you have configured DREAM.3D, simply build DREAM.3D in your preferred IDE, or open the Terminal app and build DREAM.3D without an IDE.

---
**Next Page**: [Creating a Filter](http://dream3d.bluequartz.net/binaries/Help/DREAM3D/creating_a_filter.html)

**Previous Page**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_Linux.md">Making an SDK</a>