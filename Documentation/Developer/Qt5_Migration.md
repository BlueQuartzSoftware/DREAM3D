# Qt 5 Migration #

## Qt 5.4.0 ##

Go to [Qt.io](http://www.qt.io) and download the Qt 5.4.0 MSVC OpenGL 64 Bit VS2013 build of Qt 5.4.0. We choose to install into the following location

    C:/DREAM3D_SDK/Qt5.4.0

Once you have Qt installed you will also want to install the following helper for VS2013 debugging.

[http://anychimirror101.mirrors.tds.net/pub/Qt/archive/vsaddin/qt-vs-addin-1.2.4-opensource.exe](VS Addin 1.2.4)

## Qwt Issues ##


+ Download version 6.1.2 for Qt 5.4 compatibility
+ Modify the following files:


#### Edit qwt-6.1.2/src/src.pro ####

This is only for OS X to build a proper framework.

replace the following line:

	CONFIG += lib_bundle

with this:

	CONFIG += lib_bundle
	QMAKE_LFLAGS_SONAME  *= -install_name $${QWT_INSTALL_PREFIX}/lib/qwt.framework/Versions/6/qwt

This will set correctly the full path to the framework that is built. If this is **NOT** performed then the final DREAM3D Packaging will not work correctly.

#### Edit qwt-6.1.2/qwtconfig.pri ####

replace the following line:

	QWT_INSTALL_PREFIX    = /usr/local/qwt-$$QWT_VERSION

with this (OS X):

	QWT_INSTALL_PREFIX    = /Users/Shared/Toolkits/qwt-$$QWT_VERSION

or this (Windows):

    QWT_INSTALL_PREFIX    = C:/DREAM3D_SDK/Qwt-$$QWT_VERSION

Adjust the path to place Qwt where all the other DREAM3D library dependencies are at.

### Building Qwt 6.1.2 ###

Use qmake from the Qt 5.4 distribution from the command line with the following commands executed from inside the qwt-6.1.2 directory

	qmake qwt.pro

and then for OS X:

	make
	make install

For Windows it is probably (From a Visual Studio X64 command prompt)

	nmake
	nmake install


## Updating your DREAM3D_SDK.cmake File ##

 You will want to update your DREAM3D_SDK.cmake file with the following or similar. You will probably need to adjust the paths for your system. The example below is for a Windows System.

    set(BUILD_SHARED_LIBS ON CACHE BOOL "")
    set(DREAM3D_SDK_ROOT "C:/DREAM3D_SDK")
    set(DREAM3D_DATA_DIR ${DREAM3D_SDK_ROOT}/DREAM3D_Data CACHE PATH "")
    if(0)
    	set(QT_QMAKE_EXECUTABLE "${DREAM3D_SDK_ROOT}/qt-4.8.5/bin/qmake.exe" CACHE FILEPATH "")
    	set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/qwt-5.2.2" CACHE PATH "")
    else()
    	set(Qt5Core_DIR "${DREAM3D_SDK_ROOT}/Qt5.4/5.4/msvc2013_64_opengl/lib/cmake/Qt5Core" CACHE PATH "")
    	set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/Qwt-6.1.2 CACHE PATH "")
    endif()
    
    set(EIGEN_INSTALL "${DREAM3D_SDK_ROOT}/Eigen-3.1.2" CACHE PATH "")
    set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/qwt-5.2.2" CACHE PATH "")
    set(DOXYGEN_EXECUTABLE "${DREAM3D_SDK_ROOT}/doxygen-1.8.7/doxygen.exe" CACHE FILEPATH "")
    set(HDF5_INSTALL "${DREAM3D_SDK_ROOT}/hdf5-1.8.12" CACHE PATH "")
    set(BOOST_ROOT "${DREAM3D_SDK_ROOT}/boost-1.55.0" CACHE PATH "")
    set(TBB_INSTALL_DIR  "${DREAM3D_SDK_ROOT}/tbb42_20140601oss" CACHE PATH "")
    set(DREAM3D_USE_MULTITHREADED_ALGOS ON CACHE BOOL "")
    set(TBB_ARCH_TYPE "intel64" CACHE STRING "")
    set(ITK_DIR "${DREAM3D_SDK_ROOT}/ITK-4.7.0")
    set(DREAM3D_EXTRA_PLUGINS "ImageProcessing")
    if(1)
    	set(DREAM3D_EXTRA_PLUGINS "ImageProcessing;BrukerIntegration;DictionaryIndexing;NETLIntegration;TransformationPhase;ProcessModeling;ZeissImport" CACHE STRING "")
    endif()



# Building DREAM3D #

One needs to "export" the CMAKE_PREFIX_PATH environment variable in order to find Qt 5. On OS X I did the following:

	export CMAKE_PREFIX_PATH=/Users/Shared/Toolkits/Qt-5.4.0/5.4/clang_64/lib/cmake/Qt5

Where my Qt 5.4 was installed into

	/Users/Shared/Toolkits/Qt-5.4.0

Once you have that then one can run the usual CMake configuration.



