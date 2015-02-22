#!/bin/bash

PARALLEL_BUILD=8


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

SDK_INSTALL=/Users/Shared/Toolkits

cd $SDK_INSTALL


CMAKE=`type -P cmake`
if [[ $CMAKE == "" ]];
  then
  echo "CMake is needed for this script. Please install it on your system and be sure it is on your path."
  exit 1
fi

# Build the HDF5 libraries we need and set our Environment Variable.

if [ ! -e "$SDK_INSTALL/hdf5-1.8.14.tar.gz" ];
then
  echo "-------------------------------------------"
  echo " Downloading HDF5 Version 1.8.14 "
  echo "-------------------------------------------"
  $DOWNLOAD_PROG  "http://www.hdfgroup.org/ftp/HDF5/current/src/hdf5-1.8.14.tar.gz" -o hdf5-1.8.14.tar.gz
fi

if [ ! -e "$SDK_INSTALL/hdf5-1.8.14_source" ];
then
  tar -xvzf hdf5-1.8.14.tar.gz
  mv hdf5-1.8.14 hdf5-1.8.14_source
fi
# We assume we already have downloaded the source for HDF5 Version 1.8.7 and have it in a folder
# called hdf5-188
cd hdf5-1.8.14_source
mkdir Build
cd Build
cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug -DHDF5_BUILD_WITH_INSTALL_NAME=ON -DHDF5_BUILD_CPP_LIB=ON -DHDF5_BUILD_HL_LIB=ON -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/hdf5-1.8.14 ../
make -j
make install
cd ../
mkdir zRel
cd zRel
cmake  -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DHDF5_BUILD_WITH_INSTALL_NAME=ON -DHDF5_BUILD_CPP_LIB=ON -DHDF5_BUILD_HL_LIB=ON -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/hdf5-1.8.14 ../
make -j
make install
export HDF5_INSTALL=$SDK_INSTALL/hdf5-1.8.14



