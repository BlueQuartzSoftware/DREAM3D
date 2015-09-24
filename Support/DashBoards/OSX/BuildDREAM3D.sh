#!/bin/bash

# Setup some paths that we need
export CMAKE_INSTALL=/Users/Shared/Toolkits/cmake-3.0.1/CMake.app/Contents/
export QTDIR=/Users/Shared/Toolkits/Qt-4.8.5-Cocoa
export GIT_INSTALL=/usr/local/git
export PATH=${CMAKE_INSTALL}/bin:${QTDIR}/bin:${GIT_INSTALL}/bin:$PATH

cd /Users/Shared/Dashboards

rm -rf DREAM3D
git clone git://github.com/dream3d/DREAM3D.git

cd DREAM3D
git checkout attributeMatrix
git pull --rebase origin attributeMatrix

cd ../

# Checkout the master branch of TransformationPhase Repository
rm -rf TransformationPhase
git clone git://github.com/dream3d/TransformationPhase.git

#--- Set the DREAM3D_SDK to the current directory. This is all hard coded
function writeInitialCache()
{
	DREAM3D_SDK_ROOT=/Users/Shared/Toolkits
	echo "DREAM3D_SDK:PATH=/Users/Shared/Toolkits" > $1
	echo "BUILD_SHARED_LIBS:BOOL=ON" >> $1
	echo "DREAM3D_DATA_DIR:PATH=${DREAM3D_SDK_ROOT}/DREAM3D_Data" >> $1
	echo "QT_QMAKE_EXECUTABLE:FILEPATH=${DREAM3D_SDK_ROOT}/Qt-4.8.5-Cocoa/bin/qmake" >> $1
	echo "EIGEN_INSTALL:PATH=${DREAM3D_SDK_ROOT}/Eigen-3.1.2" >> $1
	echo "QWT_INSTALL:PATH=${DREAM3D_SDK_ROOT}/Qwt-5.2.2" >> $1
	echo "HDF5_INSTALL:PATH=${DREAM3D_SDK_ROOT}/hdf5-1.8.13" >> $1
	echo "BOOST_ROOT:PATH=${DREAM3D_SDK_ROOT}/boost-1.55.0" >> $1
	echo "TBB_INSTALL_DIR:PATH=${DREAM3D_SDK_ROOT}/tbb42_20140601oss" >> $1
	echo "SIMPL_USE_MULTITHREADED_ALGOS:BOOL=ON" >> $1
	echo "TBB_ARCH_TYPE:STRING=intel64" >> $1
	echo "ITK_DIR:PATH=${DREAM3D_SDK_ROOT}/ITKBuild" >> $1
	echo "DREAM3D_EXTRA_PLUGINS:STRING=ImageProcessing;TransformationPhase" >> $1

	echo "CTEST_SITE:STRING=OSX1068.bluequartz.net" >> $1
	echo "CTEST_BUILD_NAME:STRING=OSX1068-Xcode326" >> $1
#	echo "CTEST_CONFIGURATION_TYPE:STRING=Debug" >> $1
	echo "CTEST_CMAKE_GENERATOR:STRING=Unix Makefiles" >> $1
}


# Build a Release Version
buildType=Release
rm -rf DREAM3D_${buildType}
mkdir DREAM3D_${buildType}
cd DREAM3D_${buildType}
writeInitialCache CMakeCache.txt
cmake -DCMAKE_BUILD_TYPE=${buildType}  ../DREAM3D
ctest -D Nightly
make -j2 package

if [ ! -d /Volumes/DREAM3D_Public_Builds ]; then

	mkdir /Volumes/DREAM3D_Public_Builds
	mount -t smbfs //guest:@thor.local./DREAM3D_Public_Builds /Volumes/DREAM3D_Public_Builds

fi

cp DREAM3D-*.tar.gz /Volumes/DREAM3D_Public_Builds/.
#umount /Volumes/DREAM3D_Public_Builds
cd ../

# Build a Debug Version
buildType=Debug
rm -rf DREAM3D_${buildType}
mkdir DREAM3D_${buildType}
cd DREAM3D_${buildType}
writeInitialCache CMakeCache.txt
cmake -DCMAKE_BUILD_TYPE=${buildType}  ../DREAM3D
ctest -D Nightly
make -j2 package
cd ../








