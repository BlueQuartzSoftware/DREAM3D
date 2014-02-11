#!/bin/bash

# This script will download and install the following projects and compile and install them into
# the location determined by the current working directory.
# The packages downloaded are:
# CMake - in order to build other projects
# MXABoost - subset of Boost 1.44
# Qwt - 2D Plotting Widgets for the Qt toolkit
# You need to have the following programs already on your system:
#  curl or wget
#  git version 1.7 or greater
#  Qt version 4.5 or greater
# The script will create 2 directories as subdirectories of the current working dir: sources and Toolkits
# All projects are downloaded and compiled into sources and then installed into Toolkits.
# A bash style shell file can be called from your .bashrc or .bash_profile called 'initvars.sh' which
# will export a number of paths so that building projects from the IPHelper project goes smoother.
# You can simply 'source "initvars.sh" any time to get the exports.

# If you want to force another directory then change where "SDK_INSTALL" points to.

SDK_SOURCE=/home/mjackson/Workspace
SDK_INSTALL=/home/mjackson/DREAM3D_SDK

# If you are on a Multicore system and want to build faster set the variable to the number
# of cores/CPUs available to you.
PARALLEL_BUILD=2

HOST_SYSTEM=`uname`
echo "Host System: $HOST_SYSTEM"
# Adjust these to "0" if you want to skip those compilations. The default is to build
# everything.
BUILD_CMAKE="0"
BUILD_MXABOOST="0"
BUILD_HDF5="1"
BUILD_QWT="0"

make_command=gmake
if [[ "$HOST_SYSTEM" = "Darwin" ]];
  then
  make_command=make
fi

GIT=`type -P git`
if [[ $GIT == "" ]];
  then
  echo "Git is needed for this script. Please install it on your system and be sure it is on your path."
  exit 1
fi


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

# Create some directories for our use
mkdir -p $SDK_SOURCE
mkdir -p $SDK_INSTALL
cd $SDK_SOURCE

DOWNLOAD_PROG=""
DOWNLOAD_ARGS=""

if [[ "$WGET" != "" ]];
then
  DOWNLOAD_PROG=$WGET
fi

if [[ "$CURL" != "" ]];
then
  DOWNLOAD_PROG=$CURL
  DOWNLOAD_ARGS="-o cmake-2.8.7.tar.gz"
fi

if [[ "$BUILD_CMAKE" == "1" ]];
then
#Download and Compile CMake
$DOWNLOAD_PROG "http://www.cmake.org/files/v2.8/cmake-2.8.7.tar.gz" $DOWNLOAD_ARGS
tar -xvzf cmake-2.8.7.tar.gz
cd cmake-2.8.7
./configure --prefix=$SDK_INSTALL/cmake-2.8.7 --parallel $PARALLEL_BUILD
$make_command -j $PARALLEL_BUILD install

# Export these variables for our use and then echo them into a file that people can use
# to setup their environment
export CMAKE_INSTALL=$SDK_INSTALL/cmake-2.8.7
export PATH=$CMAKE_INSTALL/bin:$PATH

echo "export CMAKE_INSTALL=$SDK_INSTALL/cmake-2.8.6" >  $SDK_INSTALL/initvars.sh
echo "export PATH=\$CMAKE_INSTALL/bin:\$PATH" >>  $SDK_INSTALL/initvars.sh
fi

#------------------------------------------------------------------------------
cd $SDK_SOURCE
git clone git://scm.bluequartz.net/CMP.git CMP


if [ "$BUILD_MXABOOST" == "1" ];
then
#------------------------------------------------------------------------------
# We now need MXABoost on the system
cd $SDK_SOURCE
# Remove any previous MXABoost
#rm -rf MXABoost
git clone git://scm.bluequartz.net/MXABoost.git MXABoost
cd MXABoost
git checkout master
mkdir Build
cd Build
cmake -DBOOST_INCLUDE_INSTALL_DIR=include/boost-1_44 -DBOOST_LIB_INSTALL_DIR=lib -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/MXABoost-1.44 -DENABLE_SHARED=OFF -DWINMANGLE_LIBNAMES=ON ../
$make_command -j $PARALLEL_BUILD install
export BOOST_ROOT=$SDK_INSTALL/MXABoost-1.44
echo "export BOOST_ROOT=$SDK_INSTALL/MXABoost-1.44" >> $SDK_INSTALL/initvars.sh
fi


if [ "$BUILD_HDF5" == "1" ];
then
# Build the HDF5 libraries we need and set our Environment Variable.
cd $SDK_SOURCE
if [ ! -e "$SDK_SOURCE/hdf5-1.8.8.tar.gz" ];
then
$DOWNLOAD_PROG  "http://www.hdfgroup.org/ftp/HDF5/prev-releases/hdf5-1.8.8/src/hdf5-1.8.8.tar.gz" -o hdf5-1.8.8.tar.gz
fi

if [ ! -e "$SDK_SOURCE/hdf5-1.8.8" ];
then
tar -xvzf hdf5-1.8.8.tar.gz
fi
# We assume we already have downloaded the source for HDF5 Version 1.8.7 and have it in a folder
# called hdf5-188
cd hdf5-1.8.8
mkdir Build
cd Build
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/hdf5-188 -DCMAKE_BUILD_TYPE=Debug  -DHDF5_ENABLE_DEPRECATED_SYMBOLS=OFF ../
make -j$makeJobs
make install
cd ../
mkdir zRel
cd zRel
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/hdf5-188 -DCMAKE_BUILD_TYPE=Release   -DHDF5_ENABLE_DEPRECATED_SYMBOLS=OFF ../
make -j$makeJobs
make install
export HDF5_INSTALL=$SDK_INSTALL/hdf5-188
fi


QMAKE=`type -P qmake`
if [[ $QMAKE == "" ]];
  then
  echo "An installation of Qt is required. Please install a version of Qt of at least 4.6 or greater."
  exit 1
fi



if [ "$BUILD_QWT" = "1" ];
then
#------------------------------------------------------------------------------
cd $SDK_SOURCE
# Remove any previous Qwt
#rm -rf Qwt
git clone --recursive git://scm.bluequartz.net/Qwt.git Qwt
cd Qwt
mkdir Build
cd Build
# On OS X we need to set the "install_name" correctly on Libraries that will get used which is
# what the variable will do.
if [ "$HOST_SYSTEM" = "Darwin" ];
  then
  ADDITIONAL_ARGS="-DCMP_BUILD_WITH_INSTALL_NAME=ON"
fi
echo "Additional Args: $ADDITIONAL_ARGS"
cmake $ADDITIONAL_ARGS -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/Qwt -DBUILD_SHARED_LIBS=ON ../
$make_command -j $PARALLEL_BUILD
$make_command install
cd ../
mkdir zRel
cd zRel
cmake $ADDITIONAL_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/Qwt -DBUILD_SHARED_LIBS=ON ../
$make_command -j $PARALLEL_BUILD
$make_command install
export QWT_INSTALL=$SDK_INSTALL/Qwt
echo "export QWT_INSTALL=$SDK_INSTALL/Qwt" >> $SDK_INSTALL/initvars.sh
fi

cd $SDK_SOURCE
git clone  ssh://code@scm.bluequartz.net/DREAM3D.git
cd DREAM3D
git fetch
git checkout master
git pull origin master
mkdir Build
cd Build
cmake $ADDITIONAL_ARGS -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/DREAM3D -DBUILD_SHARED_LIBS=OFF ../
$make_command -j $PARALLEL_BUILD
$make_command install
cd ../
mkdir zRel
cd zRel
cmake $ADDITIONAL_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/DREAM3D -DBUILD_SHARED_LIBS=OFF ../
$make_command -j $PARALLEL_BUILD
$make_command install
export DREAM3D_INSTALL=$SDK_INSTALL/DREAM3D
echo "export DREAM3D_INSTALL=$SDK_INSTALL/DREAM3D" >> $SDK_INSTALL/initvars.sh




