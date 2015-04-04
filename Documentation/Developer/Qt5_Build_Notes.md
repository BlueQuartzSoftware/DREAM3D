# Building Qt5 on Windows #

# Important Notes Before Going Forward #

Building Qt5 is NON Trivial and requires intimate knowledge of several build systems and languages. The DREAM3D Developers **strongly** urge you to simply download the latest prebuilt binary of Qt from [Qt.io](http://www.qt.io). The ONLY reason to compile your own version of Qt5 is if you can not use Visual Studio 2013 on Windows (assuming you are developing on Windows.). The complication of building Qt5 is the QtWebKit which depends on the ICU libraries which are currently extremely difficult to build your self (Requires MinGW/Cygwin/MSys). DREAM3D may eventually depend on QtWebKit to render HTML documentation pages instead of using the web browser on the users system. If you, as the developer, chose to NOT compile QtWebKit, you may lose the ability to compile DREAM3D itself at some later date. You have been warned. 

# Moving On… #
These instructions are for building Qt 5 on windows for the DREAM3D SDK deployment. We are going to use Visual Studio (VS) versions:

+ 2012 x64 toolset (xxx)
+ 2013 x64 toolset (12.0.30723.00 Update 3)

**These instructions are NOT guaranteed to even work.**

## Downloading Source Codes ##

Get the source code from the Qt website [http://download.qt-project.org/official_releases/qt/5.3/5.4.1/single/qt-everywhere-opensource-src-5.4.1.zip](http://download.qt-project.org/official_releases/qt/5.3/5.4.1/single/qt-everywhere-opensource-src-5.4.1.zip)

## Prepping for the Build ##

Qt5 now **requires** the use of Python, Perl and Ruby for the builds. I have used the Enthought Canopy Express install of Python and the ActiveState Perl 5.x installer for both of these.

+ [http://storage-cf.enthought.com/installers/Canopy/canopy-1.4.1-win-64.msi](http://storage-cf.enthought.com/installers/Canopy/canopy-1.4.1-win-64.msi)
+ [http://www.activestate.com/activeperl/downloads](http://www.activestate.com/activeperl/downloads)
+[http://rubyinstaller.org/](http://rubyinstaller.org/)

In all cases get the 64 bit versions of the tools.

## Configuring the Build ##

This is a good resource:
[http://qt-project.org/doc/qt-5/windows-building.html](http://qt-project.org/doc/qt-5/windows-building.html)

We want the build to go into a completely separate build directory so we can get that directory into the DREAM3D SDK instead of the complete source codes. We need *debug* and *release* builds for Qt5 in all cases.


We are going to use C:\Developer\x64\ as our main working directory. Open a Visual Studio 20XX x64 command prompt so that Perl, Python and Ruby are all on the path and available. I will assume you have already downloaded the Qt5 sources as a big zip file and have already

    cd C:\Developer\x64
    mkdir Qt-5.4.1
    mkdir Qt-5.4.1-Build
    set QT_INSTALL_PREFIX=C:\Developer\x64\Qt-5.4.1
    set PATH=%PATH%;C:\Developer\x64\Qt-5.4.1-Build\qtbase\bin
    cd Qt-5.4.1-Build
    
    
    ..\qt-everywhere-opensource-src-5.4.1\configure -confirm-license -opensource -debug-and-release -shared -platform win32-msvc20XX -no-c++11 -opengl desktop -largefile -mp -no-strip -prefix %QT_INSTALL_PREFIX%

**make sure to set the -platform to the proper value on that command**

This is basically a **Full Build** and can take several **HOURS** to complete. Give it time. Go get lunch or coffee or do some work. 

Run the command which will build qmake and a few other items and then generate all the project files.

    nmake install
    nmake docs
    nmake install_docs



