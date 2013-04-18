Compiling DREAM.3D {#compilingdream3d}
========

## External Dependencies

- A Compiler suite for your Operating System (Visual Studio, Xcode, GCC)
- CMake version 2.8.5
- Git for checking out source code from the repositories
- Qt Version 4.8.4 or greater (Tested with Qt 4.8.4)
- Qwt Library (2D Plots)
- HDF5 Version 1.8.10.1
- Boost Version 1.44
- Eigen Version 3.1.2


## Creating the Documentation
 If you would like to create the API or other documentation you will need to install
DOxygen and optionally LaTex for your system.

## Order of Installation

### CMake
  Download CMake from http://www.cmake.org/cmake/resources/software.html and install onto your system

### Git
  Download Git from http://git-scm.com/ and install onto your system

### Qt
Download Qt from http://qt.nokia.com. One can usually download and install the "Qt SDK" which will have everything you need. **Windows Visual Studio Users** Nokia provides pre-built binaries for Qt for Visual Studio (VS) BUT they are ONLY 32 bit and they are ONLY for Visual Studio 2008 and ONLY for a specific version of Visual Studio 2008. If your version of Visual Studio does NOT match theirs, down to the exact sercurity patches and service packs you **will** get errors due to "manifests", C/C++ runtime DLL issues and a few other things. If you fall into the category of mismatching versions the easiest, though initially longest, solution is to build Qt from scratch. Or if you need 64 bit support.

### Building Qt on Windows
Download the source code from qt.nokia.com and decompress the archive into the final location where you want Qt. You **CAN NOT** move the Qt installation **AFTER** you finish building it so choose wisely. Once you have your location selected open up a "Visual Studio Command Prompt" for either a 32 bit or 64 bit build. Once the Command Prompt is opened, navigate to the source direction and issue the following command

    configure -nomake demos -nomake examples -debug-and-release -shared -fast -exceptions -stl -no-qt3support

and answer a few of the questions. That will run for a short while after which you can issue the command

    nmake

to build the rest of Qt. **Warning**Completely compiling Qt can take 8 HOURS even on a very fast computer. Allow enough time to get this step completed.

### Building Qwt
Use Git to clone the Qwt source code repository:

    git clone git://scm.bluequartz.net/support-libraries/qwt.git qwt
    cd qwt

 And use CMake to configure Qwt for your development environment. After CMake has generated the project files, open the IDE or type "make install" on the command line to build and install Qwt

### Building HDF5
 Download the latest HDF5 Version 1.8.x from the HDF Group website at http://www.hdfgroup.org/HDF5/release/obtain5.html
Compile and Install HDF5 into a location of your choice. Export the "HDF5_INSTALL" environment variable to make configuring DREAM3D with CMake easier.

### Building Boost
Download Boost from the official http://www.boost.org website or build from a subset of boost that is provided by the BlueQuartz Git repository at http://www.bluequartz.net/gitweb/?p=MXABoost.git;a=summary

Use CMake to configure Boost for your development environment. After CMake has generated the project files, open the IDE or type "make install" on the command line to build and install Boost

### Building Eigen
Download Eigen from [Eigen Home Page](http://eigen.tuxfamily.org/index.php?title=Main_Page) The project is configured with CMake and there is actually no compiling to do for Eigen. Just compfigure and then execute the "install" target for your IDE or makefiles. That is all for Eigen

## Building DREAM.3D
After you have acquired the source code for DREAM.3D you can use CMake to configure it for your development environment and then build it. Several CMake variables will need to be filled out correctly in order for CMake to properly find all the libraries that you just built or installed onto your computer.

    HDF5_INSTALL - The location of the HDF5 installation
    BOOST_ROOT - The location of the Boost installation
    QWT_INSTALL - Location of the Qwt installation
    QT_QMAKE_EXECUTABLE - The location of the QMake executable from the Qt installation



## Building a Release of DREAM.3D

 These instructions are for those wanting to create an official distribution of DREAM.3D for the main web site. Here is the list of things to do in the order they need to be performed:

- Open the Top level CMakeLists.txt file and adjust the _CMP_VERSION_MAJOR_ to the desired version for this release. Refer to the documentation for determining the version number and how/what to set it to.
- Save the CMakeLists.txt file
- Peform one last "git commit ...." to commit this version number.
- Run CMake to generate new sources with the latest version number.
- Compile DREAM.3D one last time.
- Create the downloadable package by either running the **PACKAGE** project in Visual Studio or running _make package_ on Makfile based systems.
- Upload the binaries to the website
- Update the main web page with the new version and download links.
- Update the rss feed file with the new version and download links.



