#!/bin/bash
# For users of this script you will need to adjust the following variables:

# This is where all the projects are going to be checked out and built
workspace=`pwd`
workspace=`dirname $workspace`

# Set this to where you want to install all the libraries and executables. Each 
# project will be installed into its own sub-directory under that directory
CMAKE_INSTALL_PREFIX="/tmp/Toolkits"

# The number of concurrent compiles:
parallel=16

# to make this work with your own setup. You will need the following projects in your path:
#
# CMake
# Git
# Qt
# If any of those are missing then this script will fail horribly.

GIT=`type -P git`
if [ $GIT == "" ]
  then
  echo "Git is needed for this script. Please install it on your system and be sure it is on your path."
  exit 1
fi

QMAKE=`type -P qmake`
if [ $QMAKE == "" ]
  then
  echo "An installation of Qt is required. Please install a version of Qt of at least 4.6 or greater."
  exit 1
fi

CMAKE=`type -P cmake`
if [ $CMAKE == "" ]
  then
  echo "An installation of CMake is required. Please install a version of CMake of at least 2.8.3 or greater."
  exit 1
fi

# Export the location of CMake into our path.
CMAKE_BIN_DIR=`dirname $CMAKE`
export PATH=$PATH:$CMAKE_BIN_DIR


# Move up a directory in order to build everything here
cd ../

# We are building from the command line so Makefiles work best. I have not tried it 
# with any other CMake generator
CMAKE_GENERATOR="Unix Makefiles"


# clone all the projects
git clone --recursive git://scm.bluequartz.net/support-libraries/hdf5-v1-6.git hdf5-169
git clone --recursive git://scm.bluequartz.net/support-libraries/mxaboost.git MXABoost
git clone --recursive git://scm.bluequartz.net/support-libraries/qwt.git Qwt


# $1 Name of Project Directory
# $2 CMake Buld Type
# $3 Build Directory
# $4 CMake Install Prefix
# $5 UPPER Case version of Project Abbreviation (MXA, HDF5)
# $6 CMake Generator Type
function CompileAndInstallProject()
{
  cd $workspace/$1
  git pull origin master
  CMAKE_BUILD_TYPE=$2
  echo "CompileAndInstallProject() - Creating $2 version of $1"
  # rm -rf $3
  mkdir $3
  cd $workspace/$1/$3
  cmake -DCMP_BUILD_WITH_INSTALL_NAME=ON -DCMAKE_INSTALL_PREFIX=$4 -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -G "$6" ../
  cmake -DCMAKE_OSX_ARCHITECTURES="i386;x86_64" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.5 -DCMAKE_OSX_SYSROOT=/Developer/SDKs/MacOSX10.5.sdk ../
  make -j$parallel install 
}

function CompileAndInstallBoostCMake()
{
  cd $workspace/$1
  git pull origin master
  echo "Compiling Boost..... "
  # rm -rf $3
  mkdir $3
  cd $workspace/$1/$3
  cmake_args="-DCMAKE_INSTALL_PREFIX=$4"
  cmake_args="${cmake_args} -DENABLE_DEBUG=ON -DENABLE_RELEASE=ON"
  cmake_args="${cmake_args} -DENABLE_MULTI_THREADED=ON -DENABLE_SINGLE_THREADED=OFF"
  cmake_args="${cmake_args} -DENABLE_SHARED=OFF -DENABLE_STATIC=ON"
  cmake_args="${cmake_args} -DINSTALL_VERSIONED=ON"
  
  echo ${cmake_args}

  cmake $cmake_args -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -G "$6" ../
  cmake -DCMAKE_OSX_ARCHITECTURES="i386;x86_64" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.5 -DCMAKE_OSX_SYSROOT=/Developer/SDKs/MacOSX10.5.sdk ../
  make -j$parallel install
}

CompileAndInstallProject "Qwt" Release Build "$CMAKE_INSTALL_PREFIX/Qwt-5.2.2" Qwt "$CMAKE_GENERATOR"
CompileAndInstallProject "Qwt" Debug zRel "$CMAKE_INSTALL_PREFIX/Qwt-5.2.2" Qwt "$CMAKE_GENERATOR"

CompileAndInstallProject "hdf5-169" Debug Build "$CMAKE_INSTALL_PREFIX/hdf5-169" HDF5 "$CMAKE_GENERATOR"
CompileAndInstallProject "hdf5-169" Release zRel "$CMAKE_INSTALL_PREFIX/hdf5-169" HDF5 "$CMAKE_GENERATOR"

CompileAndInstallBoostCMake "MXABoost" Release Build "$CMAKE_INSTALL_PREFIX/MXABoost-1.44" MXABoost "$CMAKE_GENERATOR"

export DEV_INSTALL="$CMAKE_INSTALL_PREFIX"
export HDF5_INSTALL="$DEV_INSTALL/hdf5-169"
export QWT_INSTALL="$DEV_INSTALL/Qwt-5.2.2"
export BOOST_ROOT="$DEV_INSTALL/MXABoost-1.44"
echo "PATH=$PATH:$HDF5_INSTALL/bin"

CompileAndInstallProject  "AIMRepresentation" Release Build "$CMAKE_INSTALL_PREFIX/AIMRepresentation" AIMRepresentation "$CMAKE_GENERATOR"

echo "****************************************************************************"
echo " You may want to add the following to your .bash_profile in order to have all"
echo " libraries and executables available"

echo "export DEV_INSTALL=\"$CMAKE_INSTALL_PREFIX\""
echo "export HDF5_INSTALL=\"\$DEV_INSTALL/hdf5-169\""
echo "export QWT_INSTALL=\"\$DEV_INSTALL/Qwt-5.2.2\""
echo "export BOOST_ROOT=\"\$DEV_INSTALL/MXABoost-1.44\""
echo "PATH=\$PATH:\$HDF5_INSTALL/bin"
