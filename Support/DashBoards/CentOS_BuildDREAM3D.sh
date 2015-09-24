#!/bin/bash

# Setup some paths that we need
export CMAKE_INSTALL=/home/mjackson/DREAM3D_SDK/CMake-3.0.0
export QTDIR=/home/mjackson/DREAM3D_SDK/Qt-4.8.4


export PATH=${CMAKE_INSTALL}/bin:${QTDIR}/bin:$PATH

CMAKE=`type -P cmake`
if [[ "$CMAKE" == "" ]];
then
    echo "CMake is not found on the path"
    echo $PATH
    exit 1
fi


CMAKE="$CMAKE_INSTALL/bin/cmake"
CTEST="$CMAKE_INSTALL/bin/ctest"

# Read this file to get some other initialization values
. /home/mjackson/DREAM3D_SDK/initvars.sh



# Mount a directory from the webserver so we can copy things over when complete
# the mounting can ONLY take place as Root I think
if [ ! -e /mnt/DREAM3D_Public_Builds ];
then
    mkdir /mnt/DREAM3D_Public_Builds
fi

# Make sure the web server directory is mounted
mount.cifs //thor.bluequartz.net/DREAM3D_Public_Builds /mnt/DREAM3D_Public_Builds -o user=guest


# Move into the proper directory to start off
cd /opt/Dashboards

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
	DREAM3D_SDK_ROOT=/home/mjackson/DREAM3D_SDK
	echo "DREAM3D_SDK:PATH=/home/mjackson/DREAM3D_SDK" > $1
	echo "BUILD_SHARED_LIBS:BOOL=ON" >> $1
	echo "DREAM3D_DATA_DIR:PATH=${DREAM3D_SDK_ROOT}/DREAM3D_Data" >> $1
	echo "QT_QMAKE_EXECUTABLE:FILEPATH=${DREAM3D_SDK_ROOT}/Qt-4.8.4/bin/qmake" >> $1
	echo "EIGEN_INSTALL:PATH=${DREAM3D_SDK_ROOT}/Eigen-3.1.2" >> $1
	echo "QWT_INSTALL:PATH=${DREAM3D_SDK_ROOT}/Qwt" >> $1
	echo "HDF5_INSTALL:PATH=${DREAM3D_SDK_ROOT}/hdf5-1.8.12" >> $1
	echo "BOOST_ROOT:PATH=/usr" >> $1
	echo "TBB_INSTALL_DIR:PATH=${DREAM3D_SDK_ROOT}/tbb41_20130116oss" >> $1
	echo "TBB_ARCH_PLATFORM:STRING=${TBB_ARCH_PLATFORM}" >> $1
	echo "SIMPL_USE_MULTITHREADED_ALGOS:BOOL=ON" >> $1
	echo "ITK_DIR:PATH=${DREAM3D_SDK_ROOT}/ITK-4.5.1" >> $1
	echo "DREAM3D_EXTRA_PLUGINS:STRING=ImageProcessing;TransformationPhase" >> $1
    echo "DREAM3D_LINUX_SYSTEM=RHEL" >> $1
	echo "CTEST_SITE:STRING=CentOS6-X64.bluequartz.net" >> $1
	echo "CTEST_BUILD_NAME:STRING=CentOS-X64-GCC_447" >> $1
#	echo "CTEST_CONFIGURATION_TYPE:STRING=Debug" >> $1
	echo "CTEST_CMAKE_GENERATOR:STRING=Unix Makefiles" >> $1
}


# Build a Debug Version
buildType=Debug
rm -rf DREAM3D_${buildType}
mkdir DREAM3D_${buildType}
cd DREAM3D_${buildType}
writeInitialCache CMakeCache.txt
$CMAKE -DCMAKE_BUILD_TYPE=${buildType}  ../DREAM3D
$CTEST -D Nightly
make -j4 package
cd ../

# Build a Release Version
buildType=Release
rm -rf DREAM3D_${buildType}
mkdir DREAM3D_${buildType}
cd DREAM3D_${buildType}
writeInitialCache CMakeCache.txt
$CMAKE -DCMAKE_BUILD_TYPE=${buildType} ../DREAM3D
$CTEST -D Nightly
make -j4 package

# Copy the newly created package onto the Webserver 
cp DREAM3D-*.tar.gz /mnt/DREAM3D_Public_Builds/.

# Unmount the Webserver directory
# umount /mnt/DREAM3D_Builds

cd ../





