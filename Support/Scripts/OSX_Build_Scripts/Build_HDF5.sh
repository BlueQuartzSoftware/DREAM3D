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

# Build the HDF5 libraries we need and set our Environment Variable.
hdf5ArchiveName="hdf5-1.8.15-patch1"

if [ ! -e "$SDK_INSTALL/${hdf5ArchiveName}.tar.gz" ];
then
  echo "-------------------------------------------"
  echo " Downloading HDF5 Version 1.8.15 "
  echo "-------------------------------------------"
  $DOWNLOAD_PROG  "http://www.hdfgroup.org/ftp/HDF5/current/src/${hdf5ArchiveName}.tar.gz" -o ${hdf5ArchiveName}.tar.gz
fi

if [ ! -e "$SDK_INSTALL/${hdf5ArchiveName}" ];
then
  tar -xvzf ${hdf5ArchiveName}.tar.gz
# mv hdf5-1.8.15 hdf5-1.8.15_source
fi
# We assume we already have downloaded the source for HDF5 Version 1.8.7 and have it in a folder
# called hdf5-188
cd ${hdf5ArchiveName}
mkdir Build
cd Build
cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug -DHDF5_BUILD_WITH_INSTALL_NAME=ON -DHDF5_BUILD_CPP_LIB=ON -DHDF5_BUILD_HL_LIB=ON -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/hdf5-1.8.15-Debug ../
make -j${PARALLEL_BUILD}
make install
cd ../
mkdir zRel
cd zRel
cmake  -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DHDF5_BUILD_WITH_INSTALL_NAME=ON -DHDF5_BUILD_CPP_LIB=ON -DHDF5_BUILD_HL_LIB=ON -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/hdf5-1.8.15-Release ../
make -j${PARALLEL_BUILD}
make install
export HDF5_INSTALL=$SDK_INSTALL/hdf5-1.8.15



