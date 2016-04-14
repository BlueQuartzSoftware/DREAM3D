#!/bin/bash

#------------------------------------------------------------------------------
# Read the configuration file for the SDK Build
shopt -s extglob
configfile="SDK_Configuration.conf" # set the actual path name of your (DOS or Unix) config file
tr -d '\r' < $configfile > $configfile.unix
while IFS='= ' read lhs rhs
do
    if [[ ! $lhs =~ ^\ *# && -n $lhs ]]; then
        rhs="${rhs%%\#*}"    # Del in line right comments
        rhs="${rhs%%*( )}"   # Del trailing spaces
        rhs="${rhs%\"*}"     # Del opening string quotes 
        rhs="${rhs#\"*}"     # Del closing string quotes 
        declare $lhs="$rhs"
    fi
done < $configfile.unix
rm $configfile.unix
#------------------------------------------------------------------------------

cd $SDK_INSTALL

HOST_SYSTEM=`uname`
echo "Host System: $HOST_SYSTEM"


CMAKE=`type -P cmake`
if [[ $CMAKE == "" ]];
  then
  echo "CMake is needed for this script. Please install it on your system and be sure it is on your path."
  exit 1
fi



# if [ ! -e "$SDK_INSTALL/Eigen-${EIGEN_VERSION}_src.tar.gz" ];
# then
#     echo "-------------------------------------------"
#     echo " Downloading Eigen ${EIGEN_VERSION}"
#     echo "-------------------------------------------"
#     $DOWNLOAD_PROG "https://bitbucket.org/eigen/eigen/get/${EIGEN_VERSION}.tar.gz" -o Eigen-${EIGEN_VERSION}.tar.gz
# fi


if [ ! -e "$SDK_INSTALL/${EIGEN_FOLDER_NAME}" ];
then
    tar -xvzf ${EIGEN_ARCHIVE_NAME}
    echo "Looking for Actual Decompressed Eigen Directory because the Eigen Folks are Idiots...."
    EIGEN_DIR=`find . -type d -name 'eigen-eigen*'`
    echo "EIGEN_DIR=$EIGEN_DIR"
    mv $EIGEN_DIR ${EIGEN_FOLDER_NAME}
fi


if [ ! -e "$SDK_INSTALL/${EIGEN_FOLDER_NAME}" ];
then
    echo "Uh Oh. The Eigen Archive did not properly decompress. Exiting now."
    exit -1
fi

# We assume we already have downloaded the source for eigen and have it in a folder
cd ${EIGEN_FOLDER_NAME}
mkdir Build
cd Build
echo "SourceDirectory: $SDK_INSTALL/${EIGEN_FOLDER_NAME}" > $SDK_INSTALL/${EIGEN_FOLDER_NAME}/Build/DartConfiguration.tcl
cmake -DCMAKE_CXX_FLAGS="-stdlib=libc++ -std=c++11" -DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_DEPLOYMENT_TARGET -DCMAKE_OSX_SYSROOT=$OSX_SDK -DCMAKE_CXX_STANDARD=11 -DCMAKE_CXX_STANDARD_REQUIRED=ON -Wno-dev -DQT_QMAKE_EXECUTABLE="" -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$SDK_INSTALL/Eigen-${EIGEN_VERSION}  ../
make -j$PARALLEL_BUILD
make install

echo "" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "#--------------------------------------------------------------------------------------------------" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "# Eigen Library" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"
echo "set(EIGEN_INSTALL \"\${DREAM3D_SDK_ROOT}/Eigen-${EIGEN_VERSION}\" CACHE PATH \"\")" >> "$SDK_INSTALL/DREAM3D_SDK.cmake"

