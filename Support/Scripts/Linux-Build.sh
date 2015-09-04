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

#SDK_SOURCE=/tmp/Workspace
#SDK_INSTALL=/tmp/DREAM3D_SDK


SDK_SOURCE=$HOME/Workspace
SDK_INSTALL=$HOME/DREAM3D_SDK

# If you are on a Multicore system and want to build faster set the variable to the number
# of cores/CPUs available to you.
PARALLEL_BUILD=16

HOST_SYSTEM=`uname`
echo "Host System: $HOST_SYSTEM"
# Adjust these to "0" if you want to skip those compilations. The default is to build
# everything.
BUILD_CMAKE="0"
BUILD_BOOST="1"
BUILD_HDF5="1"
BUILD_QWT="1"
BUILD_EIGEN="1"

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


echo "set(BUILD_SHARED_LIBS OFF CACHE BOOL \"\")" > "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(DREAM3D_SDK_ROOT \"$SDK_INSTALL\")" >>  "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(DREAM3D_DATA_DIR \${DREAM3D_SDK_ROOT}/DREAM3D_Data CACHE PATH \"\")"  >>  "$SDK_INSTALL/DREAM3D_SDK.cmake"
mkdir ${SDK_INSTALL}/DREAM3D_Data

rm $SDK_INSTALL/initvars.sh

DOWNLOAD_PROG=""
DOWNLOAD_ARGS=""

if [[ "$WGET" != "" ]];
then
  DOWNLOAD_PROG=$WGET
fi

if [[ "$CURL" != "" ]];
then
  DOWNLOAD_PROG=$CURL
  DOWNLOAD_ARGS="-o cmake-2.8.12.2.tar.gz"
fi

if [[ "$BUILD_CMAKE" == "1" ]];
then
#Download and Compile CMake
$DOWNLOAD_PROG "http://www.cmake.org/files/v2.8/cmake-2.8.12.2.tar.gz" $DOWNLOAD_ARGS
tar -xvzf cmake-2.8.12.2.tar.gz
cd cmake-2.8.12.2
./configure --prefix=$SDK_INSTALL/cmake-2.8.12.2 --parallel $PARALLEL_BUILD
$make_command -j $PARALLEL_BUILD install

# Export these variables for our use and then echo them into a file that people can use
# to setup their environment
export CMAKE_INSTALL=$SDK_INSTALL/cmake-2.8.12.2
export PATH=$CMAKE_INSTALL/bin:$PATH

fi


if [[ "$BUILD_CMAKE" == "0" ]];
then

CMAKE=`type -P cmake`
if [[ $CMAKE == "" ]];
  then
  echo "CMake is needed for this script. Please install it on your system and be sure it is on your path."
  exit 1
fi

fi

#------------------------------------------------------------------------------
cd $SDK_SOURCE
if [ ! -e "$SDK_SOURCE/CMP" ];
  then
  echo "-------------------------------------------"
  echo " Cloning CMP project for Qwt compile"
  echo "-------------------------------------------"
  git clone git://scm.bluequartz.net/CMP.git CMP
fi


#https://www.threadingbuildingblocks.org/sites/default/files/software_releases/mac/tbb42_20140601oss_osx_0.tgz
# First check to see if we have already downloaded the archive file
cd $SDK_INSTALL
if [ ! -e "$SDK_INSTALL/tbb42_20140601oss_osx_0.tgz" ];
then
echo "-------------------------------------------"
echo " Downloading TBB Version tbb42_20140601 "
echo "-------------------------------------------"
$DOWNLOAD_PROG  "https://www.threadingbuildingblocks.org/sites/default/files/software_releases/mac/tbb42_20140601oss_osx.tgz" -o tbb42_20140601oss_osx.tgz
fi

# Next decompress the archive
if [ ! -e "$SDK_INSTALL/tbb42_20140601oss" ];
then
  tar -xvzf tbb42_20140601oss_osx.tgz
fi

# If we are on OS X we need to update the "install_name" of each of the TBB libraries so CMake can properly include them into the app bundle.
if [[ "$HOST_SYSTEM" = "Darwin" ]];
  then
  tbbInstallDir="$SDK_INSTALL/tbb42_20140601oss"
  cd "$tbbInstallDir/lib"
  tbbLibs="libtbb.dylib libtbbmalloc.dylib libtbb_debug.dylib libtbbmalloc_debug.dylib libtbb_preview_debug.dylib libtbb_preview.dylib"
  for l in $tbbLibs;
  do
    echo "Updating install_name for $tbbInstallDir/lib/$l"
    install_name_tool -id $tbbInstallDir/lib/$l $l
  done

fi

exit;

echo "set(TBB_INSTALL_DIR  "\${DREAM3D_SDK_ROOT}/tbb42_20140601oss" CACHE PATH \"\")" >>  "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(SIMPL_USE_MULTITHREADED_ALGOS ON CACHE BOOL \"\")"  >>  "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(TBB_ARCH_TYPE \"intel64\" CACHE STRING \"\")"  >>  "$SDK_INSTALL/DREAM3D_SDK.cmake"




if [ "$BUILD_EIGEN" == "1" ];
then

  # Build the HDF5 libraries we need and set our Environment Variable.
  cd $SDK_SOURCE
  if [ ! -e "$SDK_SOURCE/Eigen-3.1.2.tar.gz" ];
  then
    echo "-------------------------------------------"
    echo " Downloading Eigen 3.1.2 "
    echo "-------------------------------------------"
    $DOWNLOAD_PROG  "https://bitbucket.org/eigen/eigen/get/3.1.2.tar.gz" -o Eigen-3.1.2.tar.gz
  fi

  if [ ! -e "$SDK_SOURCE/Eigen-3.1.2" ];
  then
    tar -xvzf Eigen-3.1.2.tar.gz
    EIGEN_DIR=`find . -type d -name 'eigen-eigen*'`
    echo "EIGEN_DIR=$EIGEN_DIR"
    mv $EIGEN_DIR Eigen-3.1.2
  fi

  # We assume we already have downloaded the source for eigen and have it in a folder
  cd Eigen-3.1.2
  mkdir Build
  cd Build
  touch $SDK_SOURCE/Eigen-3.1.2/Build/DartConfiguration.tcl
  cmake -DQT_QMAKE_EXECUTABLE="" -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/Eigen-3.1.2  ../
  make -j$PARALLEL_BUILD
  make install

  export EIGEN_INSTALL=$SDK_INSTALL/Eigen-3.1.2

  echo "set(EIGEN_INSTALL \"\${DREAM3D_SDK_ROOT}/Eigen-3.1.2\" CACHE PATH \"\")" >>  "$SDK_INSTALL/DREAM3D_SDK.cmake"
fi


if [ "$BUILD_BOOST" == "1" ];
then
#------------------------------------------------------------------------------
# We now need Boost on the system
  cd $SDK_SOURCE
# if [ ! -e "$SDK_SOURCE/boost_1_55_0.tar.gz" ];
# then
#   echo "-------------------------------------------"
#   echo " Downloading Boost 1.55.0 "
#   echo "-------------------------------------------"
#   $DOWNLOAD_PROG  "http://softlayer-dal.dl.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.gz" -o boost_1_55_0.tar.gz
# fi

# if [ ! -e "$SDK_SOURCE/boost_1_55_0" ];
# then
# tar -xvzf boost_1_55_0.tar.gz
# cd "$SDK_SOURCE/boost_1_55_0"
# # Only Bootstrap once otherwise it takes forever each time
# ./bootstrap.sh
# fi

  if [ ! -e "$SDK_SOURCE/modular-boost" ];
    then
    echo "-------------------------------------------"
    echo " Checking out modular boost from GitHub"
    echo "-------------------------------------------"
    git clone --recursive git@github.com:boostorg/boost.git modular-boost
  fi

  cd "$SDK_SOURCE/modular-boost"
  ./bootstrap.sh
  ./b2 headers
  ./b2 -j$PARALLEL_BUILD --prefix=$SDK_INSTALL/boost --build-type=minimal --build-dir=x64 --architecture=x86 address-model=64 --with-thread install
  echo "set(BOOST_ROOT \"\${DREAM3D_SDK_ROOT}/boost\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
fi


if [ "$BUILD_HDF5" == "1" ];
then

  # Build the HDF5 libraries we need and set our Environment Variable.
  cd $SDK_SOURCE
  if [ ! -e "$SDK_SOURCE/hdf5-1.8.13.tar.gz" ];
  then
    echo "-------------------------------------------"
    echo " Downloading HDF5 Version 1.8.13 "
    echo "-------------------------------------------"
    $DOWNLOAD_PROG  "http://www.hdfgroup.org/ftp/HDF5/current/src/hdf5-1.8.13.tar.gz" -o hdf5-1.8.13.tar.gz
  fi

  if [ ! -e "$SDK_SOURCE/hdf5-1.8.13" ];
  then
    tar -xvzf hdf5-1.8.13.tar.gz
  fi
  # We assume we already have downloaded the source for HDF5 Version 1.8.7 and have it in a folder
  # called hdf5-188
  cd hdf5-1.8.13
  mkdir Build
  cd Build
  cmake cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/hdf5-1.8.13 -DHDF5_ENABLE_DEPRECATED_SYMBOLS=OFF  -DHDF5_BUILD_WITH_INSTALL_NAME=ON -DHDF5_BUILD_CPP_LIB=ON -DHDF5_BUILD_HL_LIB=ON ../
  make -j$PARALLEL_BUILD
  make install
  cd ../
  mkdir zRel
  cd zRel
  cmake cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/hdf5-1.8.13 -DHDF5_ENABLE_DEPRECATED_SYMBOLS=OFF  -DHDF5_BUILD_WITH_INSTALL_NAME=ON -DHDF5_BUILD_CPP_LIB=ON -DHDF5_BUILD_HL_LIB=ON -DHDF5_BUILD_TOOLS=ON ../
  make -j$PARALLEL_BUILD
  make install
  export HDF5_INSTALL=$SDK_INSTALL/hdf5-1.8.13

  echo "set(HDF5_INSTALL \"\${DREAM3D_SDK_ROOT}/hdf5-1.8.13\" CACHE PATH \"\")" >>  "$SDK_INSTALL/DREAM3D_SDK.cmake"
fi


QMAKE=`type -P qmake`
if [[ $QMAKE == "" ]];
  then
  echo "An installation of Qt is required. Please install a version of Qt of at least 4.8.5 or greater."
  exit 1
fi

echo "set(QT_QMAKE_EXECUTABLE "$QMAKE" CACHE FILEPATH \"\")" >>  "$SDK_INSTALL/DREAM3D_SDK.cmake"



if [ "$BUILD_QWT" = "1" ];
then
  
  cd $SDK_SOURCE
  if [ ! -e "$SDK_SOURCE/qwt-6.1.0.tar.bz2" ];
  then
    echo "-------------------------------------------"
    echo " Downloading Qwt 6.1.0"
    echo "-------------------------------------------"
    $DOWNLOAD_PROG  "http://iweb.dl.sourceforge.net/project/qwt/qwt/6.1.0/qwt-6.1.0.tar.bz2" -o qwt-6.1.0.tar.bz2
  fi
 
  if [ ! -e "$SDK_SOURCE/qwt-6.1.0.tar.bz2" ];
  then
    tar -xvzf qwt-6.1.0.tar.bz2
  fi
  
  cd "$SDK_SOURCE/qwt-6.1.0"
  
  # Change the install prefix to our 
  sed -i -e "s@\/usr\/local@${SDK_INSTALL}@g" qwtconfig.pri
  # Run QMake to generate our make files
  qmake
  $make_command -j $PARALLEL_BUILD install
  # Now export our variables and write the install path into the DREAM3D_SDK.cmake file
  export QWT_INSTALL=$SDK_INSTALL/Qwt
  echo "set(QWT_INSTALL \"\${DREAM3D_SDK_ROOT}/Qwt\" CACHE PATH \"\")" >>  "$SDK_INSTALL/DREAM3D_SDK.cmake"
fi





cd $SDK_SOURCE
if [ ! -e "$SDK_SOURCE/DREAM3D" ];
then
  echo "-------------------------------------------"
  echo " Cloning DREAM3D from GitHub"
  echo "-------------------------------------------"
  git clone https://github.com/dream3d/DREAM3D.git
fi
cd DREAM3D
git fetch
git checkout m_develop
git pull origin m_develop
echo "-------------------------------------------"
echo " Building Debug Version"
echo "-------------------------------------------"
mkdir Build
cd Build
cmake $ADDITIONAL_ARGS -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/DREAM3D -DDREAM3D_SDK=$SDK_INSTALL ../
$make_command -j $PARALLEL_BUILD

cd ../
echo "-------------------------------------------"
echo " Building Release Version"
echo "-------------------------------------------"
mkdir zRel
cd zRel
cmake $ADDITIONAL_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/DREAM3D -DDREAM3D_SDK=$SDK_INSTALL ../
$make_command -j $PARALLEL_BUILD
$make_command install
export DREAM3D_INSTALL=$SDK_INSTALL/DREAM3D


