# Qt 5 Migration #


## Qwt Issues ##


+ Download version 6.1.2 for Qt 5.4 compatibility
+ Modify the following files for OS X:

#### Edit qwt-6.1.2/qwtconfig.pri ####

replace the following line:

	QWT_INSTALL_PREFIX    = /usr/local/qwt-$$QWT_VERSION

with this:

	QWT_INSTALL_PREFIX    = /Users/Shared/Toolkits/qwt-$$QWT_VERSION

Adjust the path to place Qwt where all the other DREAM3D library dependencies are at.


#### Edit qwt-6.1.2/src/src.pro ####

This is only for OS X to build a proper framework.

replace the following line:

	CONFIG += lib_bundle

with this:

	CONFIG += lib_bundle
	QMAKE_LFLAGS_SONAME  *= -install_name $${QWT_INSTALL_PREFIX}/lib/qwt.framework/Versions/6/qwt

This will set correctly the full path to the framework that is built. If this is **NOT** performed then the final DREAM3D Packaging will not work correctly.


### Building Qwt 6.1.2 ###

Use qmake from the Qt 5.4 distribution from the command line with the following commands executed from inside the qwt-6.1.2 directory

	qmake qwt.pro

and then for OS X:

	make
	make install

For Windows it is probably (From a Visual Studio X64 command prompt)

	nmake
	nmake install

	


# Building DREAM3D #

One needs to "export" the CMAKE_PREFIX_PATH environment variable in order to find Qt 5. On OS X I did the following:

	export CMAKE_PREFIX_PATH=/Users/Shared/Toolkits/Qt-5.4.0/5.4/clang_64/lib/cmake/Qt5

Where my Qt 5.4 was installed into

	/Users/Shared/Toolkits/Qt-5.4.0

Once you have that then one can run the usual CMake configuration.



