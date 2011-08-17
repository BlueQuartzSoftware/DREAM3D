#!/bin/bash

# You need Qt Version 4.6.x already installed
# You need CMake version 2.8.3 at the minimum to build everything.
# You need both GCC and g++ installed

cd ../
export sourcedir=`pwd`

export sandbox=$HOME/DREAM3D
#export sourcedir=$HOME/Workspace

mkdir -p $sourcedir
cd $sourcedir

# Clone and Install Qwt. We assume already that at recent version
# of Qt is installed. We need at least Qt version 4.6.x
git clone git://scm.bluequartz.net/support-libraries/qwt.git qwt
cd qwt
mkdir Build
cd Build
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$sandbox/qwt -DCMAKE_BUILD_TYPE=Debug ../
make -j4
make install
cd ../
mkdir zRel
cd zRel
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$sandbox/qwt -DCMAKE_BUILD_TYPE=Release ../
make -j4
make install
export QWT_INSTALL=$sandbox/qwt

# Build the HDF5 libraries we need and set our Environment Variable.
cd $sourcedir
# We assume we already have downloaded the source for HDF5 Version 1.8.7 and have it in a folder
# called hdf5-187
cd hdf5-187
mkdir Build
cd Build
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$sandbox/hdf5-187 -DCMAKE_BUILD_TYPE=Debug -DHDF5_BUILD_HL_LIB=ON -DHDF5_ENABLE_DEPRECATED_SYMBOLS=OFF ../
make -j4
make install
cd ../
mkdir zRel
cd zRel
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$sandbox/hdf5-187 -DCMAKE_BUILD_TYPE=Release  -DHDF5_BUILD_HL_LIB=ON -DHDF5_ENABLE_DEPRECATED_SYMBOLS=OFF ../
make -j4
make install
export HDF5_INSTALL=$sandbox/hdf5-187

# Build Our Subset of Boost. Note that the user may already have Boost on their machines
# at which point we can use that installation. Just set the proper environment variable
cd $sourcedir
git clone git://scm.bluequartz.net/support-libraries/mxaboost.git mxaboost
cd mxaboost
mkdir Build
cd Build
cmake -DBOOST_INCLUDE_INSTALL_DIR=include/boost-1_44_0 -DENABLE_SHARED=OFF -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$sandbox/MXABoost -DCMAKE_BUILD_TYPE=Debug ../
make -j4
make install
export BOOST_ROOT=$sandbox/MXABoost

#Now Try and Build the Actual DREAM3D Project
cd $sourcedir
#git clone ssh://code@scm.bluequartz.net/DREAM3D.git DREAM3D
cd DREAM3D
rm -rf Build
mkdir Build
cd Build
cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$sandbox/Apps -DCMAKE_BUILD_TYPE=Release ../
make -j4
make install
