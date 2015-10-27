#!/bin/bash
# This script requires 2 arguments. The root of the DREAM3D_SDK (/Users/Shared/DREAM3D_SDK
# or /opt/DREAM3D_SDK) and the number of parallel processes to use to compile. This
# is typically 2x the number of physical cores in the machine.

SDK_INSTALL=$1

cd $SDK_INSTALL

PARALLEL_BUILD=$2


HOST_SYSTEM=`uname`
echo "Host System: $HOST_SYSTEM"

WGET=`type -P wget`
CURL=`type -P curl`

if [[ "$WGET" == "" ]];
   then
  if [[ "$CURL" == "" ]];
     then
    echo "wget and curl are NOT present on your machine. One of them is needed to download sources from the internet."
    exit 1
  fi
fi


DOWNLOAD_PROG=""
DOWNLOAD_ARGS=""

if [[ "$WGET" != "" ]];
then
  DOWNLOAD_PROG=$WGET
fi

if [[ "$CURL" != "" ]];
then
  DOWNLOAD_PROG=$CURL
  DOWNLOAD_ARGS=""
fi



CMAKE=`type -P cmake`
if [[ $CMAKE == "" ]];
  then
  echo "CMake is needed for this script. Please install it on your system and be sure it is on your path."
  exit 1
fi


EIGEN_VERSION=3.2.5


# if [ ! -e "$SDK_INSTALL/Eigen-${EIGEN_VERSION}_src.tar.gz" ];
# then
#     echo "-------------------------------------------"
#     echo " Downloading Eigen ${EIGEN_VERSION}"
#     echo "-------------------------------------------"
#     $DOWNLOAD_PROG "https://bitbucket.org/eigen/eigen/get/${EIGEN_VERSION}.tar.gz" -o Eigen-${EIGEN_VERSION}.tar.gz
# fi


if [ ! -e "$SDK_INSTALL/Eigen-${EIGEN_VERSION}_src" ];
then
    tar -xvzf Eigen-${EIGEN_VERSION}_src.tar.gz
    #echo "Looking for Actual Decompressed Eigen Directory because the Eigen Folks are Idiots...."
    #EIGEN_DIR=`find . -type d -name 'eigen-eigen*'`
    #echo "EIGEN_DIR=$EIGEN_DIR"
    #mv $EIGEN_DIR Eigen-${EIGEN_VERSION}_source
fi

# We assume we already have downloaded the source for eigen and have it in a folder
cd Eigen-${EIGEN_VERSION}_src
mkdir Build
cd Build
touch $SDK_INSTALL/Eigen-${EIGEN_VERSION}_src/Build/DartConfiguration.tcl
cmake -Wno-dev -DQT_QMAKE_EXECUTABLE="" -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/Eigen-${EIGEN_VERSION}  ../
make -j$PARALLEL_BUILD
make install

echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Eigen Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(EIGEN_INSTALL \"\${DREAM3D_SDK_ROOT}/Eigen-${EIGEN_VERSION}\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"

