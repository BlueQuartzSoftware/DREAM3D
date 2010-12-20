This toolkit was developed under United States Air Force Contract FA8650-07-D-5800.

This toolkit contains code originally developed by Dr. Mike Groeber
from the United Stated Air Force Research Laboratories.

Michael A. Jackson
BlueQuartz Software
www.bluequartz.net
mike.jackson@bluequartz.net


The software contained in this project requires the following third party libraries:
Boost (www.boost.org)
Qt (www.qtsoftware.com)

** Boost: One should use a minimum version of 1.36 in order to compile the source
codes. One can download the boost sources from www.boost.org. On all platforms the 
user should strive to compile boost as static libraries with dynamic linkage to 
the C and C++ standard libraries. 
This can be accomplished using the following scheme. 
1: First download or otherwise compile and install bjam which comes with the Boost 
source code distribution. After bjam.exe is compiled and available in your path 
open a Visual Studio Command prompt and  change directories into the top level of
the boost source distribution. 

** Windows Command Prompt invocation to compile Boost. Note that in the following
command, the "toolset=xxxxx" should match the version of Visual Studio that you
have installed on your system.
  Visual Studio 2003.Net ==> msvc-7.1
  Visual Studio 2005     ==> msvc-8.0
  Visual Studio 2008     ==> msvc-9.0
Also pay attention to the "prefix" option as that is where the final boost headers
and libraries will be installed into.
   
bjam.exe toolset=msvc-9.0 --with-program_options --with-date_time --with-thread --prefix=C:\Developer\VS9\boost_1_39 variant=release,debug threading=multi link=static runtime-link=shared install

**OS X - The following command can be used to compile boost on OS X and other Unix
like Systems. This also assumes that bjam is already compiled and ready to go.

bjam toolset=darwin --with-program_options --with-thread variant=release,debug threading=multi link=static runtime-link=shared --prefix=/Users/Shared/Toolkits/boost-1_39/ architecture=combined address-model=32_64 install


#-------------------------------------------------------------------------------
Typically the latest version of Qt is compatible with the code although for reference the
development code was written using Qt version 4.5.3 where Qt Version 4.6.1 is current
as of this writing.

Download the source code for Qt from www.qtsoftware.com. For versions of Qt up to
and including 4.5.x each platform has its own source code download which have file
names such as "qt-mac-4.5.3_src.tar.gz". For versions starting with 4.6 the file
is in a single archive file "qt-everywhere-opensource-src-4.6.0.tar". You may have
to download the source from ftp.nokia.com/qt/source in order to get a specific 
version of the sources.

Uncompress the source codes and open either "Terminal.app" on OS X, an XTerm on Linux 
or a "Visual Studio Command Prompt" on Windows. Change directories into the top 
level of the uncompressed source code and do one of the following based on your
operating system. 

-WINDOWS SPECIAL NOTE. You should locate the Qt source code directory in its FINAL
location on your file system including any naming of the top level directory. For
instance on the development system the Qt libraries are compiled into
C:\Developer\i386\Qt-4.6.1 for the 32 bit version and C:\Developer\x64\Qt-4.6.1
for the 64 bit version.
-WINDOWS SPECIAL NOTE II. In order to compile a 64 bit version of Qt you MUST be
running a 64 bit version of Windows. This is because during the compilation, the
build system needs to run programs that are built during the build process. You
will also need at least "Visual Studio Standard" edition which is the lowest edition
that supports 64 bit compiles. Visual Studion Express (as of VS 2008 Express) does
directly support 64 bit compiles.
-WINDOWS SPECIAL NOTE III. Technically it is possible to move the qt directory after
all the compiles are complete but this involves lots of unsupported hacks, work-
arounds and other "trickery" to get it to work. Google is your friend if you 
want to try this.

** Windows Visual Studio.
configure -nomake demos -nomake examples -debug-and-release -shared -fast -exceptions -stl -no-qt3-support -no-webkit 

** OS X, Linux, Unix
./configure  -nomake demos -nomake examples -debug-and-release -shared -fast -exceptions -stl -no-qt3support -no-webkit -prefix /Users/Shared/Toolkits/Qt-4.6.1-UBFramework -universal -cocoa -arch x86 -arch x86_64 -largefile

#-------------------------------------------------------------------------------
CMake
   You will need to install CMake from www.cmake.org in order to configure the
source codes for your operating system and compiler tools. Simply download CMake
from the website and use the installer to install CMake onto your system.


#-------------------------------------------------------------------------------
Compiling AIMRepresentation Sources
    You can use "git" to clone the repository onto your local system. Use the following
command:
 git clone git://echo1.bluequartz.net/AIMRepresentation.git
 
or go to www.bluequartz.net/Downloads and look for the link to the Git Repository.
Browse to the AIMRepresentation project and find the link to download a tar archive
of the source codes.

Once the source codes are downloaded onto your system launch "CMake-Gui.exe" or 
CMake-Gui.app depending on your system. This will present a Window to the user
with 2 text fields that need to be filled out:
"Where is the source code" should point to the AIMRepresentation directory:
"Where to build the binaries" should point to a DIFFERENT directory. Some developers
prefer to have CMake create a new directory located inside the AIMRepresentation
directory called "Build" or "i386" or something like that. DO NOT name it "CMake".
Trust me on that. 
  After those are filled out click the "Configure" button. A new window will appear
asking you to select the type of generator you want to run. Simply select the type
that matches the type of compile tools that you have installed. On Windows systems
for the Visual Studio choices there are both 32 and 64 bit selections. On OS X you
can have CMake generate Xcode projects in addition to plain makefiles. 
  After you have made your selection for a generator and told CMake-Gui to proceed
there will be some output printed and after a little bit the "Generate" button
will be enabled. Click the "Generate" button and wait for the process to complete.

  Notes: If the "Generate" button did NOT become enabled then there was an error
occured during the configuration step. Most likely CMake could not located either
Boost or Qt. In order to help CMake find boost you can click the "Add Entry" button
and add a new CMake variable "Boost_ROOT" with a value that points to the location
of the boost installation. To help CMake find Qt you need to manually set the 
QT_QMAKE_EXECUTABLE to the qmake executable program located in the bin directory
of the Qt installation.

  After successfully generating your project files one can open the AIMRepresentation.sln
file on Windows, AIMRepresentation.xcodeproj on OS X or if you selected to generate
makefiles you would open a terminal, navigate to the build directory and execute the
"make" command.

#-------------------------------------------------------------------------------
  After the successful compilation of AIMRepresentation there will be several 
executables available to run. Double click them to explore the code.







